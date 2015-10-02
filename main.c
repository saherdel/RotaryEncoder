/*
 * main.c
 */
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/gpio.c"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"  // for delay function 
#include "driverlib/pin_map.h" // to refer pin GPIO_PB2_
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c"
#include "inc/tm4c123gh6pm.h"



	int32_t i32Val, ui32Loop ;
	int16_t  x_value;
	uint8_t newX,  encoder, butt;
	uint8_t oldX;
	uint8_t maskx = 0b11000000;

// Initialize Serial Port

	void InitConsole(void)
	{
		 // Enable GPIO port A which is used for UART0 pins.	
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	    // Configure the pin muxing for UART0 functions on port A0 and A1.
	    // This step is not necessary if your part does not support pin muxing.
	    GPIOPinConfigure(GPIO_PA0_U0RX);
	    GPIOPinConfigure(GPIO_PA1_U0TX);
	    // Enable UART0 so that we can configure the clock.
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	    // Use the internal 16MHz oscillator as the UART clock source.
	    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC); 
	    // Select the alternate (UART) function for these pins.
	    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	    // Initialize the UART for console I/O.
	    UARTStdioConfig(0, 115200, 16000000);
	}


void read_encoder(void){
	// read encoder
	encoder =  GPIOPinRead(GPIO_PORTB_BASE, (GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5));
	// check for any change of X encoder pins
	newX = (encoder & maskx);
	// if value change
	if(newX != oldX)   
	{
		if ( ( (newX>>1)&0x40) ^ (oldX & 0x40) )
		{
			x_value++;
			// Delimit absolute max value
			if(x_value > 200) x_value = 200; 
		}
		else
		{
			x_value--;
			// Delimit absolute max value
			if(x_value < 0) x_value = 0;
		}
		// Update state
		oldX = newX; 
	}
}


int main(void){

	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5);

	InitConsole();

	while(1){
	read_encoder();
	  UARTprintf("Enc %4d, %d \n", x_value, (encoder>>5)&0x1);
	}
}
