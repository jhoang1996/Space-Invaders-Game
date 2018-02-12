// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 3/6/2015 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Timer2.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void Delay10ms(uint32_t count); // time delay in 0.01 seconds
void Wait1ms(uint32_t count); // time delay in 0.001 seconds


// *************************** Images ***************************
// enemy ship that starts at the top of the screen (arms/mouth closed)
// width=16 x height=10
const unsigned short SmallEnemy10pointA[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

// enemy ship that starts at the top of the screen (arms/mouth open)
// width=16 x height=10
const unsigned short SmallEnemy10pointB[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

// enemy ship that starts in the middle of the screen (arms together)
// width=16 x height=10
const unsigned short SmallEnemy20pointA[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

// enemy ship that starts in the middle of the screen (arms apart)
// width=16 x height=10
const unsigned short SmallEnemy20pointB[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000,
 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};



// enemy ship that starts at the bottom of the screen (arms down)
// width=16 x height=10
const unsigned short SmallEnemy30pointA[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

// enemy ship that starts at the bottom of the screen (arms up)
// width=16 x height=10
const unsigned short SmallEnemy30pointB[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

// image of the player's ship
// includes two blacked out columns on the left and right sides of the image to prevent smearing when moved 2 pixels to the left or right
// width=18 x height=8
const unsigned short PlayerShip0[] = {
 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0,
 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0,
 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0,
 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0,
 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0,
 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x07E0, 0x07E0, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,


};

const unsigned short PlayerShipB[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,


};

// small shield floating in space to cover the player's ship from enemy fire (undamaged)
// width=18 x height=5
const unsigned short Bunker0[] = {
 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F, 0x079F,
 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x079F,
 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F,
 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F,
 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000, 0x0000, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F,
 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x079F, 0x0000, 0x0000,

};

// width=2 x height=4
const unsigned short Missile[] = {
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};




// *************************** Capture image dimensions out of BMP**********

#define fresh 1
#define stale 0
#define N 	  20

int32_t Avg;
int32_t Data;        // 12-bit ADC
int32_t Position;    // 32-bit fixed-point 0.001 cm
uint32_t ADCMail;
uint32_t ADCStatus;

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void SysTick_Init(void);
void SystTick_Handler(void);

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	volatile uint32_t delay;
//Turn on clock for port f
	SYSCTL_RCGCGPIO_R |= 0x20;
//Wait for clock to stabilize
	delay = SYSCTL_RCGCGPIO_R;
//Setup PF3-1
	GPIO_PORTF_DIR_R   |= 0x0E;
	GPIO_PORTF_DEN_R 	 |= 0x0E;
	GPIO_PORTF_AFSEL_R &= ~0x0E;
	GPIO_PORTF_PUR_R 	 |= 0x0E;
}

// Initialize Port E so PE0, PE1 are buttons
void PortE_Init(void){
	volatile uint32_t delay;
//Turn on clock for port e
	SYSCTL_RCGCGPIO_R |= 0x10;
//Wait for clock to stabilize
	delay = SYSCTL_RCGCGPIO_R;
//Setup PE1-0
	GPIO_PORTE_DIR_R   &= ~0x03;
	GPIO_PORTE_DEN_R 	 |= 0x03;
	GPIO_PORTE_AFSEL_R &= ~0x03;
	GPIO_PORTE_PUR_R 	 &= ~0x03;
}

// Initialize Port B so PB3-0 are for the DAC
void PortB_Init(void){
	volatile uint32_t delay;
/*//Turn on clock for port b
	SYSCTL_RCGCGPIO_R |= 0x02;
//Wait for clock to stabilize
	delay = SYSCTL_RCGCGPIO_R;
//Setup PB3-0
	GPIO_PORTE_DIR_R   &= ~0x03;
	GPIO_PORTE_DEN_R 	 |= 0x03;
	GPIO_PORTE_AFSEL_R &= ~0x03;
	GPIO_PORTE_PUR_R 	 &= ~0x03;*/
	//Turn on clock for port b
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // 1) activate port B
	//Wait for clock to stabilize
	delay = SYSCTL_RCGC2_R; // 2) allow time to finish activating
	//Setup PB3-0
  GPIO_PORTB_DIR_R |= 0x0F;    // 5) outputs on PB3-0
	GPIO_PORTB_DEN_R |= 0x0F;    // 7) enable digital on PB3-0
  GPIO_PORTB_AFSEL_R &= ~0x0F; // 6) regular function on PB3-0
	GPIO_PORTB_DR8R_R |= 0x0F;   // 8) enable 8ma control register
}

void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;    					// disable SysTick during setup
  NVIC_ST_CTRL_R = 0x07;
  NVIC_ST_RELOAD_R = 2000000;			// reload value --> 40 Hz
  NVIC_ST_CURRENT_R = 0;      		// any write to current clears it
}

int tickCounter;

void SysTick_Handler(void){
//Toggle Heartbeat Twice
	PF2 ^= 0xFF;          
  PF2 ^= 0xFF;
//Sample ADC
	for(int i = 0; i < N; i++){
		ADCMail += ADC_In();
	}
	ADCMail = (ADCMail/N);
//Set Flag
	ADCStatus = fresh;
	PF2 ^= 0xFF;
	tickCounter = tickCounter + 1;
}

int32_t Convert(uint32_t input){
  int32_t pos = input;
	pos = (input + 211) / 1000;
	return pos - 2;
}

int flag;

int aliens[5][6];
int state;
int alienXPos;
int alienYPos;
int direction;
int shipPos;
int shipSpeed;
int score;
int lives;
int gameover;
int missileXPos;
int missileYPos;
int missileSpeed;
int fireButtonPressed;
int fireButtonReleased;
int pauseButtonPressed;
int pauseButtonReleased;
int lazerXPos[6];
int lazerYPos[6];
int lazerSpeed;
int difficulty;
int slide;

int emptyColumn(int c) {
	int isEmpty = 1;
	for (int r = 0; r < 5 && isEmpty; r++) {
		isEmpty = !aliens[r][c];
	}
	return isEmpty;
}

int emptyRow(int r) {
	int isEmpty = 1;
	for (int c = 0; c < 6 && isEmpty; c++) {
		isEmpty = !aliens[r][c];
	}
	return isEmpty;
}

int alienFirstColumn() {
	int start = 0;
	while (start < 6 && emptyColumn(start)) {
		start = start + 1;
	}
	return start;
}

int alienLastColumn() {
	int end = 5;
	while (end >= 0 && emptyColumn(end)) {
		end = end - 1;
	}
	return end;
}

int alienWidth() {
	int start = alienFirstColumn();
	int end = alienLastColumn();
	if (start == 6)
		return 0;
	return end - start + 1;
}

int alienHeight() {
	int start = 0;
	while (start < 5 && emptyRow(start)) {
		start = start + 1;
	}
	int end = 4;
	while (end >= 0 && emptyRow(end)) {
		end = end - 1;
	}
	if (start == 5)
		return 0;
	return end - start + 1;
}

void DrawWindow(void) {
	int first = alienFirstColumn();
	int last = alienLastColumn();
	if (tickCounter > (20 + 15 * difficulty)) {
		tickCounter = 0;
		state = !state;
		//Alien fleet has reach edge of the screen so will move down
		if ((alienXPos + alienFirstColumn() * 16 + alienWidth() * 16 + direction > 127) 
				|| (alienXPos + alienFirstColumn() * 16 + direction < 0)) {
			direction = ~direction;
			//Clears row above aliens
			ST7735_FillRect(0, alienYPos - 10, 128, 10, 0);
			alienYPos = alienYPos + 10;
			if (missileXPos != -2) {
				ST7735_DrawBitmap(missileXPos, missileYPos - 4, Missile, 2, 4);
			}
		}
		//Alien fleet has not reached an edge so will move either left or right
		else {
			//Alien fleet is moving to the right
			if (direction > 0) {
				ST7735_FillRect(alienXPos + alienFirstColumn() * 16, alienYPos - 10, 5, 60, 0);
				if (missileXPos != -2) {
						ST7735_DrawBitmap(missileXPos, missileYPos - 4, Missile, 2, 4);
					}
			}
			//Alien fleet is moving to the left
			else {
				ST7735_FillRect(alienXPos + alienFirstColumn() * 16 + alienWidth() * 16 - 6, 
					alienYPos - 10, 6, 60, 0);
					if (missileXPos != -2) {
						ST7735_DrawBitmap(missileXPos, missileYPos - 4, Missile, 2, 4);
					}
			}
			alienXPos = alienXPos + direction;
		}
	}
	/*else if (tickCounter > 300) {
		tickCounter = 0;
		int columnCount = 0;
		for (int c = 0; c < 6; c++) {
			if (!emptyColumn(c)) {
				columnCount = columnCount + 1;
			}
		}
		int max = 1;
		int pattern = 1;
		for (int counter = 0; counter < columnCount; counter++) {
			max = max * 2;
		}
		pattern = max - 1;
		for (int counter = 0; counter < 3 - difficulty; counter++) {
			pattern = (Random32() % (max - 1) + 1) & pattern;
		}
		
//		int lazerCount;
		for (int c = 0; c < 6; c++) {
			if (!emptyColumn(c)) {
				lazerXPos[c] = alienXPos + c * 16;
				int maxRow = 5;
				while (aliens[maxRow][c] == 0 && maxRow > 0) {
					maxRow = maxRow - 1;
				}
				lazerYPos[c] = alienYPos + maxRow * 10;
			}
		}
	}*/
	
	if (state == 0) {
		for (int r = 0; r < 5; r++) {
			int empty = emptyRow(r);
			for (int c = first; c <= last && !empty; c++) {
				if (aliens[r][c] == 0) {
					ST7735_FillRect(alienXPos + 16 * c, alienYPos + 10 * r - 10, 16, 10, 0);
					if (missileXPos != -2) {
						ST7735_DrawBitmap(missileXPos, missileYPos - 4, Missile, 2, 4);
					}
					for (int lazer = 0; lazer < 6; lazer++) {
						if (lazerXPos[lazer] != -2) {
							ST7735_DrawBitmap(lazerXPos[lazer], lazerYPos[lazer] - 4, Missile, 2, 4);
						}
					}
				}
				else if (aliens[r][c] == 10) {
					ST7735_DrawBitmap(alienXPos + 16 * c, alienYPos + 10 * r, 
							SmallEnemy10pointA, 16, 10);
				}
				else if (aliens[r][c] == 20) {
					ST7735_DrawBitmap(alienXPos + 16 * c, alienYPos + 10 * r,  
							SmallEnemy20pointA, 16, 10);
				}
				else if (aliens[r][c] == 30) {
					ST7735_DrawBitmap(alienXPos + 16 * c, alienYPos + 10 * r, 
							SmallEnemy30pointA, 16, 10);
				}
			}
		}
	}
	else {
		for (int r = 0; r < 5; r++) {
			int empty = emptyRow(r);
			for (int c = first; c <= last && !empty; c++) {
				if (aliens[r][c] == 0) {
					ST7735_FillRect(alienXPos + 16 * c, alienYPos + 10 * r - 10, 16, 10, 0);
					if (missileXPos != -2) {
						ST7735_DrawBitmap(missileXPos, missileYPos - 4, Missile, 2, 4);
					}
					for (int lazer = 0; lazer < 6; lazer++) {
						if (lazerXPos[lazer] != -2) {
							ST7735_DrawBitmap(lazerXPos[lazer], lazerYPos[lazer] - 4, Missile, 2, 4);
						}
					}
				}
				else if (aliens[r][c] == 10) {
					ST7735_DrawBitmap(alienXPos + 16 * c, alienYPos + 10 * r, 
							SmallEnemy10pointB, 16, 10);
				}
				else if (aliens[r][c] == 20) {
					ST7735_DrawBitmap(alienXPos + 16 * c, alienYPos + 10 * r, 
							SmallEnemy20pointB, 16, 10);
				}
				else if (aliens[r][c] == 30) {
					ST7735_DrawBitmap(alienXPos + 16 * c, alienYPos + 10 * r, 
							SmallEnemy30pointB, 16, 10);
				}
			}
		}
	}
	if (missileXPos != -2) {
		ST7735_DrawBitmap(missileXPos, missileYPos - 4, Missile, 2, 4);
	}
	/*for (int lazer = 0; lazer < 6; lazer++) {
		if (lazerXPos[lazer] != -2) {
			ST7735_DrawBitmap(lazerXPos[lazer], lazerYPos[lazer] - 4, Missile, 2, 4);
		}
	}*/
}

void CheckCollisions() {
	if (missileXPos != -2) {
		int first = alienFirstColumn();
		int last = alienLastColumn();
		for (int r = 0; r < 5; r++) {
			for (int c = first; c <= last; c++) {
				if (aliens[r][c] != 0) {
					// This implies that there is a collision with the missile and aliens[r][c]
					if (((missileXPos >= alienXPos + 16 * c) && (missileXPos < alienXPos + 16 * (c + 1)) 
								&& (missileYPos >= alienYPos + 10 * r) && (missileYPos < alienYPos + 10 * (r + 1))) ||
							((missileXPos + 1 >= alienXPos + 16 * c) && (missileXPos + 1 < alienXPos + 16 * (c + 1))
								&& (missileYPos >= alienYPos + 10 * r) && (missileYPos < alienYPos + 10 * (r + 1))) ||
							((missileXPos >= alienXPos + 16 * c) && (missileXPos < alienXPos + 16 * (c + 1))
								&& (missileYPos + 3 >= alienYPos + 10 * r) && (missileYPos + 3 < alienYPos + 10 * (r + 1))) ||
							((missileXPos + 1 >= alienXPos + 16 * c) && (missileXPos + 1< alienXPos + 16 * (c + 1))
								&& (missileYPos + 3 >= alienYPos + 10 * r) && (missileYPos + 3 < alienYPos + 10 * (r + 1)))) {
						ST7735_FillRect(missileXPos, missileYPos - 4 - missileSpeed, 2, 4 + missileSpeed, 0);
						missileXPos = -2;
						score = score + aliens[r][c];
						aliens[r][c] = 0;
						ST7735_FillRect(alienXPos + 16 * c, alienYPos + 10 * r - 10, 16, 10, 0);
						
					}
				}
			}
		}
	}
	/*for (int lazer = 0; lazer < 6; lazer++) {
		if (lazerXPos[lazer] != -2) {
			ST7735_FillRect(lazerXPos[lazer], lazerYPos[lazer] - 4 - lazerSpeed, 2, lazerSpeed - 4, 0);
		}
	}*/
}

int main(void){
	DisableInterrupts();
  TExaS_Init();  // set system clock to 80 MHz
	ADC_Init(); 
	ST7735_InitR(INITR_REDTAB); 
	PortB_Init();
	PortE_Init();
	PortF_Init();
	SysTick_Init();		
	Timer0_Init(DrawWindow, 2666667);
	Timer1_Init(CheckCollisions, 100000);
	//DAC_Init();
	Sound_Init();
  Random_Init(1);
	difficulty = 2;
	int highscore = 0;

	fireButtonReleased = 1;
	pauseButtonReleased = 1;
	
	// Initialize the alien fleet
	tickCounter = 0;
	state = 0;
	direction = 5;
	shipSpeed = 0;
	shipPos = 55;
	score = 0;
	lives = 3;
	gameover = 0;
	missileXPos = -2;
	missileYPos = -2;
	missileSpeed = 3;
	for (int i = 0; i < 6; i++) {
		lazerXPos[i] = -2;
		lazerYPos[i] = -2;
	}
	lazerSpeed = -3;
	slide = 1;
	
  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
	
	while(1) {
		
		//Start Screen
		ST7735_FillScreen(0x0000);            // set screen to black
		Delay100ms(5);              // delay .5 sec at 80 MHz
		ST7735_SetCursor(0,14);
		ST7735_OutString("   Press Any Button");
		ST7735_SetCursor(0,15);
		ST7735_OutString("     to Continue");
		int row = 13;
		tickCounter = 0;
		while(!fireButtonPressed && !pauseButtonPressed) {
			row = (row + 1);
			ST7735_SetCursor(0, row - 1);
			ST7735_OutString("                    ");
			row %= 14;
			for (int start = 0; start < 7 && !fireButtonPressed && !pauseButtonPressed; start++) {
				ST7735_SetCursor(0,row);
				for (int c = 0; c < start; c++) {
					ST7735_OutString(" ");
				}
				ST7735_OutString("SPACE INVADERS");
				int count = 0;
				while (count < 1000 && !fireButtonPressed && !pauseButtonPressed) {
					count = count + 1;
					Wait1ms(1);
					fireButtonPressed |= ((GPIO_PORTE_DATA_R & 2) >> 1);
					pauseButtonPressed |= ((GPIO_PORTE_DATA_R & 1));
				}
			}
			fireButtonPressed |= ((GPIO_PORTE_DATA_R & 2) >> 1);
			pauseButtonPressed |= ((GPIO_PORTE_DATA_R & 1));
		}
		fireButtonPressed = 0;
		fireButtonReleased = 0;
		pauseButtonPressed = 0;
		pauseButtonReleased = 0;
		EnableInterrupts();
		
		//Initialize Alien Fleet
		ST7735_FillScreen(0x0000);            // set screen to black
		Wait1ms(500);
		for (int r = 0; r < 5; r++) {
			for (int c = 0; c < 6; c++) {
				if (r == 0) {
					aliens[r][c] = 30;
				}
				else if (r == 1 || r == 2) {
					aliens[r][c] = 20;
				}
				else if (r == 3 || r == 4) {
					aliens[r][c] = 10;
				}
			}
		}
		alienXPos = 0;
		alienYPos = 29;
		Data = 2048;
		//Actual Gameplay
		while(!gameover){
			while(ADCStatus == 0) {}
			Data = Data * 9 / 10;
			Data = Data + (ADCMail / 10);		//read ADCMail (input)
			ADCStatus = 0;						//clear flag
			PF2 ^= 0x04;      // Heartbeat
			PF3 = 0x08;       // Profile Convert
			if (slide == 0) {
				Position = Convert(Data);
				shipSpeed = Position;
				shipPos = shipPos + shipSpeed;
				if (shipPos < 0) {
					shipPos = 0;
				}
				else if (shipPos > 109) {
					shipPos = 109;
				}
				ST7735_FillRect(0, 159, shipPos, 8, 0);
				ST7735_FillRect(shipPos + 18, 159, 127 - shipPos - 18, 8, 0); 
			}
			else {
				shipPos = Data * 110 / 4096;
				ST7735_FillRect(0, 159, shipPos, 8, 0);
				ST7735_FillRect(shipPos + 18, 159, 127 - shipPos - 18, 8, 0);
			}
			
			if (missileXPos != -2) {
				ST7735_FillRect(missileXPos, missileYPos - missileSpeed, 2, missileSpeed, 0);
				missileYPos = missileYPos - missileSpeed;
				ST7735_DrawBitmap(missileXPos, missileYPos - 4, Missile, 2, 4);
				if (missileYPos < 0) {
					ST7735_FillRect(missileXPos, missileYPos - 4, 2, 4, 0);
					missileXPos = -2;
				}
			}
			for (int lazer = 0; lazer < 6; lazer++) {
				if (lazerXPos[lazer] != -2) {
					ST7735_FillRect(lazerXPos[lazer], lazerYPos[lazer] + missileSpeed, 2, missileSpeed, 0);
					lazerYPos[lazer] = lazerYPos[lazer] + missileSpeed;
					ST7735_DrawBitmap(lazerXPos[lazer], lazerYPos[lazer] + 4, Missile, 2, 4);
					if (lazerYPos[lazer] > 160) {
						ST7735_FillRect(lazerXPos[lazer], lazerYPos[lazer] + 4, 2, 4, 0);
						missileXPos = -2;
					}
				}
			}
			
			PF3 = 0;          // end of Convert Profile
			PF1 = 0x02;       // Profile LCD
			ST7735_SetCursor(0,0);
			ST7735_OutString("Score:");
			LCD_OutDec(score);
			ST7735_SetCursor(13,0);
			ST7735_OutString("Lives:");
			LCD_OutDec(lives);
			PF1 = 0;   
			
			fireButtonReleased |= fireButtonPressed && !((GPIO_PORTE_DATA_R & 2) >> 1);
			fireButtonPressed = ((GPIO_PORTE_DATA_R & 2) >> 1);
			pauseButtonReleased |= pauseButtonPressed && !((GPIO_PORTE_DATA_R & 1));
			pauseButtonPressed = ((GPIO_PORTE_DATA_R & 1));
			if (missileXPos == -2 && fireButtonPressed) {
				Sound_Shoot();
				missileXPos = shipPos + 8;
				missileYPos = 154;
			}
			
			
			if (pauseButtonPressed && pauseButtonReleased) {
				DisableInterrupts();
				pauseButtonPressed = 0;
				pauseButtonReleased = 0;
				fireButtonPressed = 0;
				fireButtonReleased = 0;
				ST7735_FillScreen(0x0000); 
				ST7735_SetCursor(0,0);
				ST7735_OutString("Score:");
				LCD_OutDec(score);
				ST7735_SetCursor(13,0);
				ST7735_OutString("Lives:");
				LCD_OutDec(lives);
				
				int option = 0;
				int exit = 0;
				while (!exit) {
					ST7735_SetCursor(3,6);
					ST7735_OutString("Game Paused");
					if (option == 0) {
						ST7735_SetCursor(3,8);
						ST7735_OutString("  >Resume");
						ST7735_SetCursor(2,9);
						ST7735_OutString(" Difficulty:");
						ST7735_SetCursor(3,10);
						ST7735_OutString(" Slide:");
						ST7735_SetCursor(3,11);
						ST7735_OutString("    Quit");
					}
					else if (option == 1) {
						ST7735_SetCursor(3,8);
						ST7735_OutString("   Resume");
						ST7735_SetCursor(2,9);
						ST7735_OutString(">Difficulty:");
						ST7735_SetCursor(3,10);
						ST7735_OutString(" Slide:");
						ST7735_SetCursor(3,11);
						ST7735_OutString("    Quit");
					}
					else if (option == 2) {
						ST7735_SetCursor(3,8);
						ST7735_OutString("   Resume");
						ST7735_SetCursor(2,9);
						ST7735_OutString(" Difficulty:");
						ST7735_SetCursor(3,10);
						ST7735_OutString(">Slide:");
						ST7735_SetCursor(3,11);
						ST7735_OutString("    Quit");
					}
					else if (option == 3) {
						ST7735_SetCursor(3,8);
						ST7735_OutString("   Resume");
						ST7735_SetCursor(2,9);
						ST7735_OutString(" Difficulty:");
						ST7735_SetCursor(3,10);
						ST7735_OutString(" Slide:");
						ST7735_SetCursor(3,11);
						ST7735_OutString("   >Quit");
					}
					ST7735_SetCursor(14,9);
					if(difficulty == 2) {
						ST7735_OutChar('E');
					}
					else if (difficulty == 1) {
						ST7735_OutChar('M');
					} else if (difficulty == 0) {
						ST7735_OutChar('H');
					}
					
					ST7735_SetCursor(10,10);
					if (slide == 0) {
						ST7735_OutString("ACC");
					}
					else if (slide == 1) {
						ST7735_OutString("POS");
					}
					
					if (fireButtonPressed && fireButtonReleased) {
						option = (option + 1) % 4;
						fireButtonPressed = 0;
						fireButtonReleased = 0;
					}

					
					if (pauseButtonPressed && pauseButtonReleased)
					{
						pauseButtonReleased = 0;
						if (option == 0) {
							exit = 1;
						} else if (option == 1) {
							difficulty = (difficulty + 5) % 3;
						} else if (option == 2) {
							slide = (slide + 1) % 2;
						}
						else if (option == 3) {
							exit = 1;
							gameover = 1;
						}	
					}
					
					fireButtonReleased |= fireButtonPressed && !((GPIO_PORTE_DATA_R & 2) >> 1);
					fireButtonPressed = ((GPIO_PORTE_DATA_R & 2) >> 1);
					
					pauseButtonReleased |= pauseButtonPressed && !((GPIO_PORTE_DATA_R & 1));
					pauseButtonPressed = ((GPIO_PORTE_DATA_R & 1));
				}
				pauseButtonPressed = 0;
				pauseButtonReleased = 0;
				ST7735_FillScreen(0x0000); 
				EnableInterrupts();
			}
			ST7735_DrawBitmap(shipPos, 159, PlayerShip0, 18,8); // player ship middle bottom
			ST7735_FillRect(0, 151, shipPos, 8, 0);
			ST7735_FillRect(shipPos + 18, 151, 127 - shipPos - 18, 8, 0);
			gameover |= (alienYPos + alienHeight() * 10 > 159) || (lives == 0);
			
			if (alienHeight() == 0) {
				alienXPos = 0;
				alienYPos = 29;
				for (int r = 0; r < 5; r++) {
					for (int c = 0; c < 6; c++) {
						if (r == 0) {
							aliens[r][c] = 30;
						}
						else if (r == 1 || r == 2) {
							aliens[r][c] = 20;
						}
						else if (r == 3 || r == 4) {
							aliens[r][c] = 10;
						}
					}
				}
			}
		}
		if (score > highscore)
			highscore = score;
		gameover = 0;
		difficulty = 2;
		alienXPos = 1000;
		alienYPos = 1000;
		DisableInterrupts();
		
		Delay100ms(7);
		
		
		ST7735_FillScreen(0x0000);
		ST7735_SetCursor(0,0);
		ST7735_OutString("HighScore:");
		LCD_OutDec(highscore);
		ST7735_SetCursor(6,8);
		ST7735_OutString("Game Over!");
		ST7735_SetCursor(6,9);
		ST7735_OutString("Score:");
		LCD_OutDec(score);
		lives = 3;
		score = 0;
		while(!fireButtonPressed && !pauseButtonPressed) {
			fireButtonPressed |= ((GPIO_PORTE_DATA_R & 2) >> 1);
			pauseButtonPressed |= ((GPIO_PORTE_DATA_R & 1));
		}
		fireButtonPressed = 0;
		pauseButtonPressed = 0;
	}

}


// You can use this timer only if you learn how it works

void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
void Delay10ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 72724;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
void Wait1ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 7272;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
