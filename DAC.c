// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 8/25/2014 
// Last Modified: 3/6/2015 
// Section 1-2pm     TA: Wooseok Lee
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // 1) activate port B
	delay = SYSCTL_RCGC2_R; // 2) allow time to finish activating
	//SysTickWait(delay);
  GPIO_PORTB_AMSEL_R &= ~0x0F; // 3) disable analog function on PB3-0
  GPIO_PORTB_PCTL_R &= ~0x0000FFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x0F;    // 5) outputs on PB3-0
  GPIO_PORTB_AFSEL_R &= ~0x0F; // 6) regular function on PB3-0
  GPIO_PORTB_DEN_R |= 0x0F;    // 7) enable digital on PB3-0
	GPIO_PORTB_DR8R_R |= 0x0F;   // 8) enable 8ma control register
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R &= ~0x0F;
	GPIO_PORTB_DATA_R |= ((data) & 0x0F); 
}
