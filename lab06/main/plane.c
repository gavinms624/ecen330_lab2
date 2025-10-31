#include "plane.h"

enum planeState {idle_p_st, flying_st}currentState;
uint32_t wait_ticks = 0;
coord_t plane_x = LCD_W;
coord_t plane_y = PLANE_HEIGHT;
missile_t *plane_missile_g = NULL;



/******************** Plane Init Function ********************/

// Initialize the plane state machine. Pass a pointer to the missile
// that will be (re)launched by the plane. It will only have one missile.
void plane_init(missile_t *plane_missile){
    plane_x = LCD_W;
    plane_y = PLANE_HEIGHT;
    plane_missile->type = MISSILE_TYPE_PLANE;
    missile_init(plane_missile);
    currentState = idle_p_st;
    plane_missile_g = plane_missile;
}

/******************** Plane Control & Tick Functions ********************/

// Trigger the plane to explode.
void plane_explode(void){
    currentState = idle_p_st;
}

// State machine tick function.
void plane_tick(void){
    // Transition
    switch(currentState){
        case(idle_p_st):
            if(wait_ticks++ >= CONFIG_PLANE_IDLE_TIME_TICKS){
                wait_ticks = 0;
                currentState = flying_st;
                plane_x = LCD_W;
            }
            break;
        case(flying_st):
            if(plane_x <= CONFIG_PLANE_WIDTH){
                currentState = idle_st;
            }
            break;
    }
    // Action
    switch(currentState){
        case(idle_p_st):
            break;
        case(flying_st):
            if(plane_x == LAUNCH_DIST){
                missile_launch_plane(plane_missile_g, plane_x, plane_y);
            }
            lcd_fillTriangle(plane_x, plane_y, plane_x, (plane_y - CONFIG_PLANE_HEIGHT), (plane_x - CONFIG_PLANE_WIDTH), (plane_y - CONFIG_PLANE_HEIGHT/2), CONFIG_COLOR_PLANE);
            plane_x -= CONFIG_PLANE_DISTANCE_PER_TICK;
            break;
    }

}

/******************** Plane Status Function ********************/

// Return the current plane position through the pointers *x,*y.
void plane_get_pos(coord_t *x, coord_t *y){
    *x = plane_x;
    *y = plane_y;
}

// Return whether the plane is flying.
bool plane_is_flying(void){
    if(currentState == flying_st){
        return true;
    }
    return false;
}