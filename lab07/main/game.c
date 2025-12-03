#include "game.h"

coord_t x, y;
uint32_t snake_length;
uint32_t grid[COL][ROW];
uint32_t speed = 2;
uint32_t count = 0;
int32_t dx = 0;
int32_t dy = 0;
enum gameState{init_st, right_st, left_st, up_st, down_st, grow_st, collided_st}currentState;
Segment snake[MAX_LENGTH];
static const char *TAG = "lab07";

void grid_init(uint32_t grid[COL][ROW]){
    for(uint32_t c = 0; c < COL; c++){
        for(uint32_t r = 0; r < ROW; r++){
            grid[c][r] = 0;
        }
    }
    grid[snake[0].c][snake[0].r] = 1;
    //TODO: Add an apple to the grid
}

//Update LCD position of head segment
void lcd_pos_update(uint32_t pos){
    snake[pos].x = snake[pos].c * (LCD_W / COL);
    snake[pos].y = snake[pos].r * (LCD_H / ROW);
}
// Draw Snake Segment at pos
void draw_segment(uint32_t pos){
    lcd_fillRect2(snake[pos].x, snake[pos].y, snake[pos].x + SNAKE_SIZE, snake[pos].y + SNAKE_SIZE, GREEN);
}
// Clear Grid at pos
void clear_pos(uint32_t pos){
    grid[snake[pos].c][snake[pos].r] = 0;
}

// Check Bounds
bool is_collided(){
    if((snake[0].x + SNAKE_SIZE + SNAKE_SIZE*3/4) >= LCD_W || snake[0].x <= 0 || (snake[0].y + SNAKE_SIZE + SNAKE_SIZE/2) >= LCD_H || snake[0].y <= 0){
        return true;
    }
    return false;
}


// Initialize the game control logic.
void game_init(void){
    ESP_LOGI(TAG, "Application starting, version");
    snake_length = 1;
    currentState = init_st;
    snake[0].r = ROW/2;
    snake[0].c = COL/2;
    lcd_pos_update(0);
    grid_init(grid);
}

// Update the game control logic.
void game_tick(void){
    // Transitions
    switch(currentState){
        joy_get_displacement(&dx, &dy);
        case(init_st):
            // if its close to the center do nothing
            if(dx * dx + dy * dy <= MOVE_THRESHOLD * MOVE_THRESHOLD){
                break;
            }

            break;
        case(right_st):
            cursor_get_pos(&x, &y);
            if(is_collided()){
                currentState = collided_st;
            }
            if (y > (Y_CENTER + MOVE_THRESHOLD)){
                currentState = up_st;
            } else if (y < (Y_CENTER - MOVE_THRESHOLD)){
                currentState = down_st;
            }
            
            break;
        case(left_st):
            cursor_get_pos(&x, &y);
            if(is_collided()){
                currentState = collided_st;
            }
            if (y > (Y_CENTER + MOVE_THRESHOLD)){
                currentState = up_st;
            } else if (y < (Y_CENTER - MOVE_THRESHOLD)){
                currentState = down_st;
            }
            break;
        case(up_st):
            cursor_get_pos(&x, &y);
            if(is_collided()){
                currentState = collided_st;
            }
            if(x > (X_CENTER + MOVE_THRESHOLD)){
                currentState = right_st;
            } else if (x < (X_CENTER - MOVE_THRESHOLD)){
                currentState = left_st;
            }
            break;
        case(down_st):
            cursor_get_pos(&x, &y);
            if(is_collided()){
                currentState = collided_st;
            }
            if(x > (X_CENTER + MOVE_THRESHOLD)){
                currentState = right_st;
            } else if (x < (X_CENTER - MOVE_THRESHOLD)){
                currentState = left_st;
            }
            break;
        case(grow_st):
            break;
        case(collided_st):
            ESP_LOGI(TAG, "COLLIDED");
            break;
    }
    // Actions
    switch(currentState){
        case(init_st):
            draw_segment(0);
            snake_length = 1;
            break;
        case(right_st):
            // Update Head
            if(count++ >= speed){
                snake[0].c++;
                count = 0;
                clear_pos(0);
                lcd_pos_update(0);
            }
            draw_segment(0);
            if(snake_length <= 1){
                break;
            }
            // Draw body, not tail end
            for(uint32_t i = 2; i < (snake_length-1); i++){
                grid[snake[i-1].c][snake[i-1].r] = grid[snake[i].c][snake[i].r];
                lcd_pos_update(i);
                draw_segment(i);
            }
            // Take care of tail
            clear_pos(snake_length-1);
            // snake[snake_length-1].c;
            lcd_pos_update(snake_length-1);

            // Draw
            draw_segment(snake_length-1);
            break;
        case(left_st):
            // Update Head
            if(count++ >= speed){
                snake[0].c--;
                clear_pos(0);
                lcd_pos_update(0);
                count = 0;
            }
            draw_segment(0);
            if(snake_length <= 1){
                break;
            }
            break;
        case(up_st):
            // Update Head
            if(count++ >= speed){
                snake[0].r++;
                clear_pos(0);
                lcd_pos_update(0);
                count = 0;
            }
            draw_segment(0);
            if(snake_length <= 1){
                break;
            }
            break;
        case(down_st):
            // Update Head
            if(count++ >= speed){
                snake[0].r--;
                clear_pos(0);
                lcd_pos_update(0);
                count = 0;
            }
            draw_segment(0);
            if(snake_length <= 1){
                break;
            }
            break;
        case(grow_st):
            break;
        case(collided_st):
            draw_segment(0);
            break;
    }

}