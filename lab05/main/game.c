#include "config.h"
#include "board.h"
#include "graphics.h"
#include "nav.h"
#include "com.h"
#include "game.h"
#include "hw.h"
#include "lcd.h"
#include "joy.h"
#include "pin.h"
#include <stdio.h>

#define SECOND_SCALAR 1E3f
#define X_TURN_MESSAGE "X's Turn!"
#define O_TURN_MESSAGE "O's Turn!"

int8_t r = 1;
int8_t c = 1;
enum game_state{init_st, new_game_st, wait_mark_st, mark_st, wait_restart_st} currentState;
volatile uint64_t timer_ticks;
mark_t mark = X_m;

// Initialize the game logic.
void game_init(void){
    currentState = init_st;
}

// Update the game logic.
void game_tick(void){
    // Transitions
    switch(currentState){ 
        case(init_st):
            currentState = new_game_st;
            break;
        case(new_game_st):
            currentState = wait_mark_st;
            graphics_drawMessage(X_TURN_MESSAGE, CONFIG_MESS_CLR, CONFIG_BACK_CLR);
            break;
        case(wait_mark_st):
            if(!pin_get_level(HW_BTN_A) && board_set(r, c, mark)){ 
                uint8_t loc = ((r << 4) | c); // put location in 1 byte buffer and send
                com_write(&loc, sizeof(loc));
                currentState = mark_st;
            }
            break;
        case(mark_st):
            if(board_winner(mark)){
                currentState = wait_restart_st;
                if(mark == X_m){
                    graphics_drawMessage("X Wins!", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                }else if(mark == O_m){
                    graphics_drawMessage("O Wins!", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                }
                
            } else if(!board_winner(mark) && (board_mark_count() >= CONFIG_BOARD_SPACES)){
                currentState = wait_restart_st;
                graphics_drawMessage("Draw", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
            } else {
                    currentState = wait_mark_st;
                    if(mark == X_m){
                        graphics_drawMessage(O_TURN_MESSAGE, CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                        mark = O_m;
                    } else if(mark == O_m){
                        graphics_drawMessage(X_TURN_MESSAGE, CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                        mark = X_m;
                    } else{
                        graphics_drawMessage("ERROR_X", CONFIG_MESS_CLR, CONFIG_BACK_CLR);
                        mark = X_m;
                    }
            }
            break;
        case(wait_restart_st):
            if(!pin_get_level(HW_BTN_START)){
                currentState = new_game_st;
            }
            break;
        default:
            break;
    }

    // Actions
    switch(currentState){ 
        case(init_st):
            break;
        case(new_game_st):
            mark = X_m;
            r = 1;
            c = 1;
            lcd_fillScreen(CONFIG_BACK_CLR);
            graphics_drawGrid(CONFIG_GRID_CLR);
            board_clear();
            nav_set_loc(r, c);
            break;
        case(wait_mark_st):
            if(pin_get_level(!HW_BTN_A)){ 
                nav_get_loc(&r, &c);
            }
            break;
        case(mark_st):
            if(mark == X_m){
                graphics_drawX(r, c, CONFIG_MARK_CLR);
            } else if(mark == O_m){
                graphics_drawO(r, c, CONFIG_MARK_CLR);
            }
            break;
        case(wait_restart_st):
            break;
        default:
            break;
    }
}


int main(){
    game_init();
    while(1){
        game_tick();
    }
    return 0;
}