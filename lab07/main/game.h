#ifndef GAME_H_
#define GAME_H_

#include <stdio.h>
#include <stdlib.h> // rand

#include "hw.h"
#include "lcd.h"
#include "cursor.h"
#include "pin.h"
#include "config.h"
#include "esp_log.h"

#define GRID_SIZE 30 // One dimensional size
#define ROW GRID_SIZE
#define COL ROW
#define MOVE_THRESHOLD (LCD_W/4)
#define SNAKE_SIZE (LCD_W / COL)
#define MAX_LENGTH 250
#define X_CENTER (LCD_W/2)
#define Y_CENTER (LCD_H/2)
#define SNAKE_PIXELS_PER_SEC 35
#define SNAKE_DISTANCE_PER_TICK \ (SNAKE_PIXELS_PER_SEC * CONFIG_GAME_TIMER_PERIOD)


typedef struct {
    uint32_t r;
    uint32_t c;
    uint32_t x;
    uint32_t y;
} Segment;



// Draw Snake Segment at pos
void draw_segment(uint32_t pos);
// Clear Grid at pos
void clear_pos(uint32_t pos);
// Initialize Display Position of segment
void lcd_pos_update(uint32_t pos);
// Initializes the game board. Place Snake Head at center
void grid_init(uint32_t grid[COL][ROW]);

// Initialize the game control logic.
// This function initializes all missiles, planes, stats, etc.
void game_init(void);

// Update the game control logic.
// This function calls the missile & plane tick functions, reinitializes
// idle enemy missiles, handles button presses, fires player missiles,
// detects collisions, and updates statistics.
void game_tick(void);

#endif // GAME_H_
