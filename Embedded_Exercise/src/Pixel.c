/*
 * Pixel.c
 *
 *  Created on: -----
 *      Author: -----
 */

 #include "Pixel.h"

 #define control_signal *(uint8_t *)0x41220008
 #define channel_register *(uint8_t *)0x41220000
 
 // Table for pixel dots.
 //				 dots[X][Y][COLOR]
 volatile uint8_t dots[8][8][3] = {0};
 
 // Here the setup operations for the LED matrix will be performed
 void setup()
 {
 
	 // Initialize by setting control signal and channel register to 0
	 control_signal = 0;
	 channel_register = 0;
 
	 // Resetting the screen at start (Set RSTn to 1).
	 control_signal &= ~(1 << 0);
	 usleep(500);
	 control_signal |= (1 << 0);
	 usleep(500);
 
	 // SDA bit set to 1 to send serial data.
	 // control_signal = 0b10000;
	 control_signal |= (1 << 4);
 
	 // Setting every bit in 6-bit register to 1
	 for (uint8_t a = 0; a < 144; a++)
	 {
		 // SCK should be toggled from 0 to 1
		 control_signal &= ~(1 << 3);
		 control_signal |= (1 << 3);
	 };
 
	 // Set SB-bit to 1 to enable transmission to 8-bit register.
	 control_signal |= (1 << 2);
 }
 
 // Change value of one pixel at led matrix. This function is only used for changing values of dots array
 void SetPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b)
 {
 
	 // Hint: you can invert Y-axis quite easily with 7-y
	 dots[x][y][0] = b; // blue channel
	 dots[x][y][1] = g; // green channel
	 dots[x][y][2] = r; // red channel
 }
 
 // Put new data to led matrix. Hint: This function is supposed to send 24-bytes and parameter x is for channel x-coordinate.
 void run(uint8_t x)
 {
	 // Write code that writes data to led matrix driver (8-bit data). Use values from dots array
	 // Hint: use nested loops (loops inside loops)
	 // Hint2: loop iterations are 8,3,8 (pixels,color,8-bitdata)
 
	 latch();
 
	 for (uint8_t y = 0; y < 8; y++)
	 {
		 for (uint8_t color = 0; color < 3; color++)
		 {
			 uint8_t current_color = dots[x][y][color];
			 for (uint8_t bit_count = 0; bit_count < 8; bit_count++)
			 {
				 // SDA should be set to 1 if there is still data to be sent
				 if (current_color & 0b10000000)
					 control_signal |= (1 << 4);
 
				 else
					 control_signal &= ~(1 << 4);
 
				 // SCK should be toggled from 0 to 1
				 control_signal &= ~(1 << 3);
				 control_signal |= (1 << 3);
				 // Shift left by 1
				 current_color <<= 1;
			 }
		 }
	 }
 
	 latch();
 }
 
 // Toggle the latch bit from HIGH to LOW to store the pixel data values to the register bank
 void latch()
 {
	 control_signal |= (1 << 1);
	 control_signal &= ~(1 << 1);
 }
 
 void open_line(uint8_t x)
 {
	 if (x < 8)
	 {                              // Ensure x is within the valid range (0 to 7)
		 channel_register = 1 << x; // Shift 1 left by x positions to set the correct bit
	 }
	 else
	 {
		 channel_register = 0b00000000; // Invalid x, deactivate all columns
	 }
 }
 
 // STATIC SHIP AND MOVING ALIEN
 void clear_ship(GameState *g)
 {
	 // Clear ship area on all columns (0..7). Score uses only y = 0..5 on column 7,
	 // so clearing y = ship_y / ship_y-1 will not disturb the score.
	 for (uint8_t x = 0; x < 8; x++)
	 {
		 SetPixel(x, g->ship_y, 0, 0, 0);
		 SetPixel(x, g->ship_y - 1, 0, 0, 0);
	 }
 
	//  run(g->ship_x);
	//  open_line(g->ship_x);
	//  if (g->ship_x > 1)
	//  {
	// 	 run(g->ship_x - 1);
	// 	 open_line(g->ship_x - 1);
	//  }
	//  if (g->ship_x < 6)
	//  {
	// 	 run(g->ship_x + 1);
	// 	 open_line(g->ship_x + 1);
	// }
 }
 
 void draw_ship(GameState *g)
 {
	 clear_ship(g);
	 // Row y = 7
	 // Ship is 3 pixels wide, centred at ship_x. Keep it inside columns 0..7.
	 if (g->ship_x > 0)
		 SetPixel(g->ship_x - 1, g->ship_y, 0, 0, 255);
	 SetPixel(g->ship_x, g->ship_y, 0, 0, 255);
	 if (g->ship_x < 7)
		 SetPixel(g->ship_x + 1, g->ship_y, 0, 0, 255);
 
	 // Row above
	 SetPixel(g->ship_x, g->ship_y - 1, 0, 0, 255);
 
	//  run(g->ship_x);
	//  open_line(g->ship_x);
	//  if (g->ship_x > 1)
	//  {
	// 	 run(g->ship_x - 1);
	// 	 open_line(g->ship_x - 1);
	//  }
	//  if (g->ship_x < 6)
	//  {
	// 	 run(g->ship_x + 1);
	// 	 open_line(g->ship_x + 1);
	//  }
 }
 
 void draw_alien(GameState *g)
 {
	 SetPixel(g->alien_x, g->alien_y, 255, 0, 0);
 }
 
 void clear_alien(GameState *g)
 {
	 SetPixel(g->alien_x, g->alien_y, 0, 0, 0);
 }
 
 void move_alien(GameState *g)
 {
	 clear_alien(g);
 
	 g->alien_x += g->alien_direction;
 
	 // Keep alien in playfield 0..6 (7 is score). Bounce or randomly change direction.
	 if (g->alien_x <= 0 || g->alien_x >= 6 || rand() % 35 == 0)
		 g->alien_direction = -g->alien_direction;
 
	 draw_alien(g);
 }

// BULLETS AND SCORE 

void fire_bullet(GameState *g)
{
	// Find a free bullet slot
	for (uint8_t i = 0; i < MAX_BULLETS; i++)
	{
		if (!g->bullet_active[i])
		{
			uint8_t start_y = (g->ship_y >= 2) ? (g->ship_y - 2) : 0;
			g->bullet_active[i] = 1;
			g->bullet_x[i] = g->ship_x;
			g->bullet_y[i] = start_y;

			// Draw bullet as white pixel
			SetPixel(g->bullet_x[i], g->bullet_y[i], 255, 255, 255);
			return;
		}
	}
}

static void register_miss(GameState *g)
{
	if (g->red_leds < 3)
	{
		g->red_leds++;
	}
}

static void register_hit(GameState *g)
{
	if (g->green_leds < 3)
	{
		g->green_leds++;
	}
}

void update_score_display(GameState *g)
{
	// Clear entire score column (x = 7)
	for (uint8_t y = 0; y < 8; y++)
	{
		SetPixel(7, y, 0, 0, 0);
	}

	// Draw hits (green) on top 3 pixels (y = 0..2)
	for (uint8_t i = 0; i < g->green_leds && i < 3; i++)
	{
		SetPixel(7, i, 0, 255, 0);
	}

	// Draw misses (red) on the next 3 pixels after green (y = 3,4,5)
	for (uint8_t i = 0; i < g->red_leds && i < 3; i++)
	{
		uint8_t y = 3 + i;
		SetPixel(7, y, 255, 0, 0);
	}
}

void update_bullets(GameState *g)
{
	for (uint8_t i = 0; i < MAX_BULLETS; i++)
	{
		if (!g->bullet_active[i])
			continue;

		uint8_t x = g->bullet_x[i];
		uint8_t y = g->bullet_y[i];

		// Clear current bullet pixel
		SetPixel(x, y, 0, 0, 0);

		// Move upwards
		if (y == 0)
		{
			// Bullet leaves the screen -> miss
			g->bullet_active[i] = 0;
			register_miss(g);
			continue;
		}

		uint8_t new_y = y - 1;

		// Check collision with alien
		if (x == g->alien_x && new_y == g->alien_y)
		{
			// Hit: clear alien, register hit and respawn alien
			clear_alien(g);
			register_hit(g);

			// Simple respawn near middle
			g->alien_x = 3;
			g->alien_direction = (rand() & 1) ? 1 : -1;
			draw_alien(g);

			g->bullet_active[i] = 0;
			continue;
		}

		// No hit yet, draw bullet at new position
		g->bullet_y[i] = new_y;
		SetPixel(x, new_y, 255, 255, 255);
	}

	// After bullets are updated, refresh score column
	update_score_display(g);

	// Check win / lose conditions
	if (g->green_leds >= 3)
	{
		g->game_state = GAME_WIN;
	}
	else if (g->red_leds >= 3)
	{
		g->game_state = GAME_LOSE;
	}
}

void reset_game(GameState *g)
{
	// Clear entire playfield
	for (uint8_t x = 0; x < 8; x++)
	{
		for (uint8_t y = 0; y < 8; y++)
		{
			SetPixel(x, y, 0, 0, 0);
		}
	}

	g->ship_x = 3;
	g->ship_y = 7;

	g->alien_x = 3;
	g->alien_y = 0;
	g->alien_direction = 1;

	g->green_leds = 0;
	g->red_leds = 0;
	g->game_state = GAME_PLAYING;

	for (uint8_t i = 0; i < MAX_BULLETS; i++)
	{
		g->bullet_active[i] = 0;
		g->bullet_x[i] = 0;
		g->bullet_y[i] = 0;
	}

	draw_ship(g);
	draw_alien(g);
	update_score_display(g);
}

// Draw a simple "WIN" pattern on the 8x8 matrix.
// This clears the whole matrix and then lights pixels to form letters.
void draw_win_screen(GameState *g)
{
	(void)g; // currently unused but kept for symmetry / future use

	// Clear screen
	for (uint8_t x = 0; x < 8; x++)
	{
		for (uint8_t y = 0; y < 8; y++)
		{
			SetPixel(x, y, 0, 0, 0);
		}
	}

	// Use bright green for "WIN"
	// W in columns 0-2
	SetPixel(0, 1, 0, 255, 0);
	SetPixel(0, 2, 0, 255, 0);
	SetPixel(0, 3, 0, 255, 0);
	SetPixel(1, 3, 0, 255, 0);
	SetPixel(1, 2, 0, 255, 0);
	SetPixel(2, 3, 0, 255, 0);
	SetPixel(2, 2, 0, 255, 0);
	SetPixel(2, 1, 0, 255, 0);

	// I in columns 3-4
	SetPixel(3, 1, 0, 255, 0);
	SetPixel(3, 2, 0, 255, 0);
	SetPixel(3, 3, 0, 255, 0);
	SetPixel(4, 1, 0, 255, 0);
	SetPixel(4, 2, 0, 255, 0);
	SetPixel(4, 3, 0, 255, 0);

	// N in columns 5-7
	SetPixel(5, 1, 0, 255, 0);
	SetPixel(5, 2, 0, 255, 0);
	SetPixel(5, 3, 0, 255, 0);
	SetPixel(6, 2, 0, 255, 0);
	SetPixel(7, 1, 0, 255, 0);
	SetPixel(7, 2, 0, 255, 0);
	SetPixel(7, 3, 0, 255, 0);
}

// Draw a simple "LOSE" pattern on the 8x8 matrix.
void draw_lose_screen(GameState *g)
{
	(void)g;

	// Clear screen
	for (uint8_t x = 0; x < 8; x++)
	{
		for (uint8_t y = 0; y < 8; y++)
		{
			SetPixel(x, y, 0, 0, 0);
		}
	}

	// Use bright red for "LOSE"
	// L in columns 0-1
	SetPixel(0, 1, 255, 0, 0);
	SetPixel(0, 2, 255, 0, 0);
	SetPixel(0, 3, 255, 0, 0);
	SetPixel(1, 3, 255, 0, 0);

	// O in columns 2-3
	SetPixel(2, 1, 255, 0, 0);
	SetPixel(3, 1, 255, 0, 0);
	SetPixel(2, 2, 255, 0, 0);
	SetPixel(3, 2, 255, 0, 0);
	SetPixel(2, 3, 255, 0, 0);
	SetPixel(3, 3, 255, 0, 0);

	// S in columns 4-5
	SetPixel(4, 1, 255, 0, 0);
	SetPixel(5, 1, 255, 0, 0);
	SetPixel(4, 2, 255, 0, 0);
	SetPixel(5, 3, 255, 0, 0);
	SetPixel(4, 3, 255, 0, 0);

	// E in columns 6-7
	SetPixel(6, 1, 255, 0, 0);
	SetPixel(6, 2, 255, 0, 0);
	SetPixel(6, 3, 255, 0, 0);
	SetPixel(7, 1, 255, 0, 0);
	SetPixel(7, 2, 255, 0, 0);
	SetPixel(7, 3, 255, 0, 0);
}