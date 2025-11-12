
#include <stdio.h>
#include <stdlib.h> // rand

#include "hw.h"
#include "lcd.h"
#include "cursor.h"
#include "pin.h"
#include "game.h"

int32_t snake_length;
enum gameState{init_st, right_st, left_st, up_st, down_st, grow_st, collided_st}currentState;

// Initialize the game control logic.
void game_init(void){
    snake_length = 0;
    currentState = init_st;
}

// Update the game control logic.
void game_tick(void){
    // Transitions
    switch(currentState){
        case(init_st):
            break;
        case(right_st):
            break;
        case(left_st):
            break;
        case(up_st):
            break;
        case(down_st):
            break;
    }
    // Actions
    switch(currentState){
        case(init_st):
            snake_length = 0;
            break;
        case(right_st):
            break;
        case(left_st):
            break;
        case(up_st):
            break;
        case(down_st):
            break;
    }

}