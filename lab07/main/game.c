#include "game.h"

coord_t x, y;
uint32_t snake_length;
uint32_t grid[COL][ROW];
uint32_t speed = 2;
uint32_t count = 0;
int32_t dx = 0;
int32_t dy = 0;
enum gameState{init_st, right_st, left_st, up_st, down_st, grow_st, collided_st}currentState;
enum snakeState{NONE, RIGHT, LEFT, UP, DOWN}snakedir;
Segment snake[MAX_LENGTH];
Segment apple;
static const char *TAG = "lab07";



// 1: Head 2: Snake Body 3: Apple
void grid_init(uint32_t grid[COL][ROW]){
    for(uint32_t c = 0; c < COL; c++){
        for(uint32_t r = 0; r < ROW; r++){
            grid[c][r] = 0;
        }
    }
    grid[snake[0].c][snake[0].r] = 1;
    srand(time(NULL)); 
}

void grid_update(){
    grid[snake[0].c][snake[0].r] = 1;
    for(uint32_t i = 1; i < snake_length; i++){
        grid[snake[i].c][snake[i].r] = 2;
    }
}

//Update LCD position of head segment
void lcd_pos_update(uint32_t pos){
    snake[pos].x = snake[pos].c * (LCD_W / COL);
    snake[pos].y = snake[pos].r * (LCD_H / ROW);
}

// Update LCD position of apple
void update_apple(){
    apple.c = rand() % (COL);
    apple.r = rand() % (ROW);
    grid[apple.c][apple.r] = 3;
    apple.x = apple.c * (LCD_W / COL);
    apple.y = apple.r * (LCD_H / ROW);
}


// Draw Snake Segment at pos/index in snake
void draw_segment(uint32_t pos){
    lcd_fillRect2(snake[pos].x, snake[pos].y, snake[pos].x + SNAKE_SIZE, snake[pos].y + SNAKE_SIZE, GREEN);
}

// Draw apple
void draw_apple(){
    lcd_fillRect2(apple.x, apple.y, apple.x + SNAKE_SIZE, apple.y + SNAKE_SIZE, RED);
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
    // for(uint32_t i = 1; i < snake_length; i++){
    //         if(snake[i].c == snake[0].c && snake[i].r == snake[0].r){
    //             return true;
    //         }
    // } FIX!!!!!!
    return false;
}

void move_head(){
    joy_get_displacement(&dx, &dy);
    // ESP_LOGI(TAG, "dx=%ld, dy=%ld", dx, dy);
    // if its close to the center do nothing
    if((dx * dx + dy * dy) <= (MOVE_THRESHOLD * MOVE_THRESHOLD)){
        if(currentState == init_st){
            snakedir = NONE;
        }
        return;
    }
    if(dx * dx > dy * dy){
        if(dx > 0){
            snakedir = RIGHT;
        } else{
            snakedir = LEFT;
        }
    } else{
        if(dy > 0){
            snakedir = UP;
        } else{
            snakedir = DOWN;
        }
    }

}

bool apple_eaten(){
    if(!pin_get_level(HW_BTN_A) || ((snake[0].c - apple.c) * (snake[0].c - apple.c) <= COLLISION_SENS * COLLISION_SENS && (snake[0].r - apple.r) * (snake[0].r - apple.r) <= COLLISION_SENS * COLLISION_SENS )){
        update_apple();
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
    update_apple();
    lcd_pos_update(0);
    grid_init(grid);
}

// Update the game control logic.
void game_tick(void){
    // Transitions
    switch(currentState){
        case(init_st):
            move_head();
            if(snakedir == RIGHT){
                currentState = right_st;
            } else if(snakedir == LEFT){
                currentState = left_st;
            } else if (snakedir == UP){
                currentState = up_st;
            } else if (snakedir == DOWN){
                currentState = down_st;
            }
            break;
        case(right_st):
            move_head();
            if(is_collided()){
                currentState = collided_st;
            }
            if (snakedir == UP){
                currentState = up_st;
            } else if (snakedir == DOWN){
                currentState = down_st;
            } else if(apple_eaten()){
                currentState = grow_st;
            }
            
            break;
        case(left_st):
            move_head();
            if(is_collided()){
                currentState = collided_st;
            }
            if (snakedir == UP){
                currentState = up_st;
            } else if (snakedir == DOWN){
                currentState = down_st;
            } else if(apple_eaten()){
                currentState = grow_st;
            }
            
            break;
        case(up_st):
            if(is_collided()){
                currentState = collided_st;
            }
            move_head();
            if(snakedir == RIGHT){
                currentState = right_st;
            } else if(snakedir == LEFT){
                currentState = left_st;
            } else if(apple_eaten()){
                currentState = grow_st;
            }

            break;
        case(down_st):
            // cursor_get_pos(&x, &y);
            if(is_collided()){
                currentState = collided_st;
            }
            move_head();
            if(snakedir == RIGHT){
                currentState = right_st;
            } else if(snakedir == LEFT){
                currentState = left_st;
            }else if(apple_eaten()){
                currentState = grow_st;
            }

            break;
        case(grow_st):
            move_head();
            if(snakedir == RIGHT){
                currentState = right_st;
            } else if(snakedir == LEFT){
                currentState = left_st;
            } else if (snakedir == UP){
                currentState = up_st;
            } else if (snakedir == DOWN){
                currentState = down_st;
            }
            break;
        case(collided_st):
            if(!pin_get_level(HW_BTN_START)){
                update_apple();
                currentState = init_st;
            }
            break;
    }
    // Actions
    switch(currentState){
        case(init_st):
            draw_segment(0);
            snake_length = 1;
            snake[0].r = ROW/2;
            snake[0].c = COL/2;
            lcd_pos_update(0);
            draw_apple();
            break;
        case(right_st):
            // Update Body
            if(count++ >= speed){
                // Draw body, not tail end
                for(uint32_t i = (snake_length-1); i > 0; i--){
                    snake[i].c = snake[i-1].c;
                    snake[i].r = snake[i-1].r;
                    lcd_pos_update(i);
                }

                // Update Head
                clear_pos(0);
                snake[0].c++;
                lcd_pos_update(0);
                grid_update();
                count = 0;
            }
            for(uint32_t i = 0; i < snake_length; i++){
                draw_segment(i);
            }
            draw_apple();
            break;
        case(left_st):
            // Update Body
            if(count++ >= speed){
                // Draw body, not tail end
                for(uint32_t i = (snake_length-1); i > 0; i--){
                    snake[i].c = snake[i-1].c;
                    snake[i].r = snake[i-1].r;
                    lcd_pos_update(i);
                }

                // Update Head
                clear_pos(0);
                snake[0].c--;
                lcd_pos_update(0);
                grid_update();
                count = 0;
            }
            for(uint32_t i = 0; i < snake_length; i++){
                draw_segment(i);
            }
            draw_apple();
            break;
        case(up_st):

            // Update Body
            if(count++ >= speed){
                // Draw body, not tail end
                for(uint32_t i = (snake_length-1); i > 0; i--){
                    snake[i].c = snake[i-1].c;
                    snake[i].r = snake[i-1].r;
                    lcd_pos_update(i);
                }

                // Update Head
                clear_pos(0);
                snake[0].r++;
                lcd_pos_update(0);
                grid_update();
                count = 0;
            }
            for(uint32_t i = 0; i < snake_length; i++){
                draw_segment(i);
            }
            draw_apple();
            break;
        case(down_st):
            // Update Body
            if(count++ >= speed){
                // Draw body, not tail end
                for(uint32_t i = (snake_length-1); i > 0; i--){
                    snake[i].c = snake[i-1].c;
                    snake[i].r = snake[i-1].r;
                    lcd_pos_update(i);
                }

                // Update Head
                clear_pos(0);
                snake[0].r--;
                lcd_pos_update(0);
                grid_update();
                count = 0;
            }
            for(uint32_t i = 0; i < snake_length; i++){
                draw_segment(i);
            }
            draw_apple();
            break;
        case(grow_st):
            if (snake_length + GROW_SPEED < MAX_LENGTH) {
                for(uint32_t i = 0; i < GROW_SPEED; i++){
                    snake[snake_length + i].c = snake[snake_length - 1].c; // Place all new tails on top of current tail until we move and update
                    snake[snake_length + i].r = snake[snake_length - 1].r;
                    lcd_pos_update(snake_length + i); 
                }
                snake_length += GROW_SPEED;
            }
            ESP_LOGI(TAG, "Length = %ld", snake_length);
            break;
        case(collided_st):
            for(uint32_t i = 0; i < snake_length; i++){
                draw_segment(i);
            }
            break;
    }

}