#include "game.h"

coord_t x, y;
uint32_t snake_length;
uint32_t grid[COL][ROW];
enum gameState{init_st, right_st, left_st, up_st, down_st, grow_st, collided_st}currentState;
Segment snake[MAX_LENGTH];

void grid_init(uint32_t grid[COL][ROW]){
    for(uint32_t c = 0; c < COL; c++){
        for(uint32_t r = 0; r < ROW; r++){
            grid[c][r] = 0;
        }
    }
    grid[snake[0].c][snake[0].r] = 1;
    //TODO: Add an apple to the grid
}

void segment_xy_init(uint32_t pos){
    snake[pos].x = snake[pos].c * (LCD_W / COL);
    snake[pos].y = snake[pos].r * (LCD_H / ROW);
}

// Initialize the game control logic.
void game_init(void){
    printf("HELLO");
    snake_length = 0;
    currentState = init_st;
    snake[0].r = ROW/2;
    snake[0].c = COL/2;
    segment_xy_init(0);
    grid_init(grid);
}

// Update the game control logic.
void game_tick(void){
    cursor_get_pos(&x, &y);
    // Transitions
    switch(currentState){
        case(init_st):
            printf("DRAWING SNAKE at (%ld,%ld)\n", snake[0].x, snake[0].y);
            // if(x > MOVE_THRESHOLD){
            //     currentState = right_st;
            // } else if (x < (-1*MOVE_THRESHOLD)){
            //     currentState = left_st;
            // } else if (y > (MOVE_THRESHOLD)){
            //     currentState = up_st;
            // } else if (y < (-1 * MOVE_THRESHOLD)){
            //     currentState = down_st;
            // }
            break;
        case(right_st):
            if (y > (MOVE_THRESHOLD)){
                currentState = up_st;
            } else if (y < (-1 * MOVE_THRESHOLD)){
                currentState = down_st;
            }
            break;
        case(left_st):
            if (y > (MOVE_THRESHOLD)){
                currentState = up_st;
            } else if (y < (-1 * MOVE_THRESHOLD)){
                currentState = down_st;
            }
            break;
        case(up_st):
            if(x > MOVE_THRESHOLD){
                currentState = right_st;
            } else if (x < (-1*MOVE_THRESHOLD)){
                currentState = left_st;
            }
            break;
        case(down_st):
            if(x > MOVE_THRESHOLD){
                currentState = right_st;
            } else if (x < (-1*MOVE_THRESHOLD)){
                currentState = left_st;
            }
            break;
        case(grow_st):
            break;
        case(collided_st):
            break;
    }
    // Actions
    switch(currentState){
        case(init_st):
            
            lcd_fillRect2(snake[0].x, snake[0].y, snake[0].x + SNAKE_SIZE, snake[0].y + SNAKE_SIZE, GREEN);
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
        case(grow_st):
            break;
        case(collided_st):
            break;
    }

}