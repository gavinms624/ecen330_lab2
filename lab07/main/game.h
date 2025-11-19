#ifndef GAME_H_
#define GAME_H_

#include <stdio.h>
#include <stdlib.h> // rand

#include "hw.h"
#include "lcd.h"
#include "cursor.h"
#include "pin.h"
#include "config.h"

#define ROW 30
#define COL ROW
#define MOVE_THRESHOLD 50
#define SNAKE_SIZE 50
#define MAX_LENGTH 250


typedef struct {
    uint32_t r;
    uint32_t c;
    uint32_t x;
    uint32_t y;
} Segment;


// Initialize Display Position of segment
void segment_xy_init(uint32_t pos);
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
