/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

/*
 *
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

// Main program for exercise

//****************************************************
// By default, every output used in this exercise is 0
//****************************************************
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xgpiops.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xparameters.h"
#include "Pixel.h"
#include "Interrupt_setup.h"

//********************************************************************
//***************TRY TO READ COMMENTS*********************************
//********************************************************************

//***Hint: Use sleep(x)  or usleep(x) if you want some delays.****
//***To call assembler code found in blinker.S, call it using: blinker();***

// GLOBAL VARIABLES
volatile uint8_t channel = 0;
GameState game = {0};

// Comment this if you want to disable all interrupts
#define enable_interrupts

/***************************************************************************************
Name: Huyen Pham
Student number: 153151026

Name: Trang Vu
Student number:

Tick boxes that you have coded

Led-matrix driver		Game		    Assembler
	[x]					[x]					[]

Brief description:

*****************************************************************************************/

int main()
{
	//**DO NOT REMOVE THIS****
	init_platform();
	//************************

#ifdef enable_interrupts
	init_interrupts();
#endif

	// setup screen
	setup();

	// Initialise full game state and draw initial entities / score
	reset_game(&game);

	Xil_ExceptionEnable();

	// Try to avoid writing any code in the main loop.
	while (1)
	{
	}

	cleanup_platform();
	return 0;
}

// Timer interrupt handler for led matrix update. Frequency is 800 Hz
void TickHandler(void *CallBackRef)
{
	// Don't remove this
	uint32_t StatusEvent;

	// Exceptions must be disabled when updating screen
	Xil_ExceptionDisable();
	//****Write code here ****

	// Refresh LED matrix
	open_line(8);

	run(channel);
	open_line(channel);

	channel++;

	if (channel >= 8)
	{
		channel = 0;
	}

	//****END OF OWN CODE*****************

	//*********clear timer interrupt status. DO NOT REMOVE********
	StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
	XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
	//*************************************************************
	// enable exceptions
	Xil_ExceptionEnable();
}

// Timer interrupt for moving alien, shooting... Frequency is 10 Hz by default
void TickHandler1(void *CallBackRef)
{

	// Don't remove this
	uint32_t StatusEvent;

	//****Write code here ****
	if (game.game_state == GAME_PLAYING)
	{
		move_alien(&game);
		update_bullets(&game);
	}
	else if (game.game_state == GAME_WIN)
	{
		// Show win screen when player has enough hits
		draw_win_screen(&game);
	}
	else if (game.game_state == GAME_LOSE)
	{
		// Show lose screen when player has too many misses
		draw_lose_screen(&game);
	}

	//****END OF OWN CODE*****************
	// clear timer interrupt status. DO NOT REMOVE
	StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
	XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
}

// Interrupt handler for switches and buttons.
// Reading Status will tell which button or switch was used
// Bank information is useless in this exercise
void ButtonHandler(void *CallBackRef, u32 Bank, u32 Status)
{
	//****Write code here ****

	// Hint: Status bit meanings:
	// 0x01 -> BTN0, 0x02 -> BTN1, 0x04 -> BTN2, 0x08 -> BTN3,
	// 0x10 -> SW0, 0x20 -> SW1

	// BTN0 (bit 0): move right (only while playing)
	if ((Status & 0x01) && game.game_state == GAME_PLAYING)
	{
		// Keep ship within game field (x = 1..6, ship width is 3; side can reach column 7)
		if (game.ship_x < 6)
		{
			game.ship_x++;
			draw_ship(&game);
		}
	}

	// BTN1 (bit 1): move left (only while playing)
	else if ((Status & 0x02) && game.game_state == GAME_PLAYING)
	{
		if (game.ship_x > 1)
		{
			game.ship_x--;
			draw_ship(&game);
		}
	}

	// BTN2 (bit 2): fire while playing
	else if (Status & 0x04)
	{
		if (game.game_state == GAME_PLAYING)
		{
			fire_bullet(&game);
		}
	}

	// BTN3 (bit 3): always restart
	else if (Status & 0x08)
	{
		reset_game(&game);
	}

	//****END OF OWN CODE*****************
}
