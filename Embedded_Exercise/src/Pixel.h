/*
 * Pixel.h
 *
 *  Created on: ------
 *      Author: ------
 */

 #ifndef PIXEL_H_
 #define PIXEL_H_
 
 #include "platform.h"
 #include "xil_printf.h"
 #include "sleep.h"
 #include "xgpiops.h"
 #include "xttcps.h"
 #include "xscugic.h"
 #include "xparameters.h"
 #include <stdlib.h>
 
#define control_signal *(uint8_t*) 0x41220008
#define channel_register *(uint8_t*) 0x41220000

#define MAX_BULLETS 3

// game_state values
#define GAME_PLAYING 0
#define GAME_WIN     1
#define GAME_LOSE    2
 
 typedef struct {
     uint8_t ship_x;
     uint8_t ship_y;
 
     uint8_t alien_x;
     uint8_t alien_y;
     int8_t  alien_direction;
 
     uint8_t green_leds;
     uint8_t red_leds;
 
     uint8_t game_state;

    // Simple bullet pool
    uint8_t bullet_active[MAX_BULLETS];
    uint8_t bullet_x[MAX_BULLETS];
    uint8_t bullet_y[MAX_BULLETS];
 } GameState;
 
 void setup();
 void SetPixel(uint8_t x,uint8_t y, uint8_t r, uint8_t g, uint8_t b);
 void run(uint8_t x);
 void latch();
 void open_line(uint8_t x);
 void draw_ship(GameState* g);
 void clear_ship(GameState* g);
 void draw_alien(GameState* g);
 void clear_alien(GameState* g);
 void move_alien(GameState* g);
void fire_bullet(GameState* g);
void update_bullets(GameState* g);
void update_score_display(GameState* g);
void reset_game(GameState* g);
void draw_win_screen(GameState* g);
void draw_lose_screen(GameState* g);
 #endif /* PIXEL_H_ */
 