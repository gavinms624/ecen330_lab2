#include "missile.h"



#define FIRING_LOC_1 (LCD_W/4)
#define FIRING_LOC_2 (2*LCD_W/4)
#define FIRING_LOC_3 (3*LCD_W/4)
#define YMAX (LCD_H/4)
#define XMAX (LCD_W-1)





/******************** Missile Init & Launch Functions ********************/

// Different _launch_ functions are used depending on the missile type.

// Initialize the missile as an idle missile. When initialized to the idle
// state, a missile doesn't appear nor does it move. The launch flag should
// also be set to false. Other missile_t members will be set up at launch.
void missile_init(missile_t *missile){
    missile->currentState = idle_st;
    missile->launch = false;
}


// Helper function for launch functions
static void finalize_launch(missile_t *missile){
    float dy = (float)missile->y_dest - (float)missile->y_origin;
    float dx = (float)missile->x_dest - (float)missile->x_origin;
    missile->length = 0;
    missile->explode_me = false;
    missile->total_length = sqrtf(dy * dy + dx * dx);
    missile->launch = true;
    missile->x_current = missile->x_origin;
    missile->y_current = missile->y_origin;
}

// Launch the missile as a player missile. This function takes an (x, y)
// destination of the missile (as specified by the user). The origin is the
// closest "firing location" to the destination (there are three firing
// locations evenly spaced along the bottom of the screen).
void missile_launch_player(missile_t *missile, coord_t x_dest, coord_t y_dest){
    coord_t x_orig;
    coord_t y_orig = LCD_H;

    // Find Closest Firing Location
    if(x_dest < FIRING_LOC_1 || ((x_dest - FIRING_LOC_1) < (FIRING_LOC_2 - x_dest))){
        x_orig = FIRING_LOC_1;
    } else if((x_dest > FIRING_LOC_1 && x_dest < FIRING_LOC_3) && (x_dest <= FIRING_LOC_2 || ((x_dest - FIRING_LOC_2) < (FIRING_LOC_3 - x_dest)))){
        x_orig = FIRING_LOC_2;
    } else{
        x_orig = FIRING_LOC_3;
    }

    missile->type = MISSILE_TYPE_PLAYER;
    missile->x_dest = x_dest;
    missile->y_dest = y_dest;
    missile->x_origin = x_orig;
    missile->y_origin = y_orig;
    finalize_launch(missile);
}

// Launch the missile as an enemy missile. This will randomly choose the
// origin and destination of the missile. The origin is somewhere near the
// top of the screen, and the destination is the very bottom of the screen.
void missile_launch_enemy(missile_t *missile){
    coord_t x_origin = rand() % (XMAX + 1);
    coord_t y_origin = rand() % (YMAX + 1);
    coord_t x_dest = rand() % (XMAX + 1);
    coord_t y_dest = LCD_H;

    missile->type = MISSILE_TYPE_ENEMY;
    missile->x_dest = x_dest;
    missile->y_dest = y_dest;
    missile->x_origin = x_origin;
    missile->y_origin = y_origin;
    finalize_launch(missile);
}

// Launch the missile as a plane missile. This function takes the (x, y)
// location of the plane as an argument and uses it as the missile origin.
// The destination is randomly chosen along the bottom of the screen.
void missile_launch_plane(missile_t *missile, coord_t x_orig, coord_t y_orig){
    coord_t x_dest = rand() % (XMAX + 1);
    coord_t y_dest = LCD_H;

    missile->type = MISSILE_TYPE_PLANE;
    missile->x_dest = x_dest;
    missile->y_dest = y_dest;
    missile->x_origin = x_orig;
    missile->y_origin = y_orig;
    finalize_launch(missile);
}


/******************** Missile Control & Tick Functions ********************/

// Used to indicate that a moving missile should be detonated. This occurs
// when an enemy or a plane missile is located within an explosion zone.
void missile_explode(missile_t *missile){
    missile->explode_me = true;
}

// Tick the state machine for a single missile.
void missile_tick(missile_t *missile){
    float fraction = 0;
    // Transition
    switch(missile->currentState){
        case(idle_st):
            if(missile->launch){
                missile->currentState = moving_st;
            }
            break;
        case(moving_st):
            if(missile->explode_me == true){
                missile->currentState = exploding_grow_st;
            } else if(missile_is_impacted(missile)){
                missile->currentState = impacted_st;
            }
            break;
        case(exploding_grow_st):
            if(missile->radius >= CONFIG_EXPLOSION_MAX_RADIUS){
                missile->currentState = exploding_shrink_st;
            }
            break;
        case(exploding_shrink_st):
            if(missile->radius <= 0){
                missile->currentState = idle_st;
            }
            break;
        case(impacted_st): 
            missile->currentState = idle_st;
            break;
    }

    // Action
    switch(missile->currentState){
        case(idle_st):
            missile->launch = false;
            missile->length = 0;
            fraction = 0;
            break;
        case(moving_st):
            if(missile->type == MISSILE_TYPE_ENEMY){
                missile->length += CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
                fraction = missile->length / missile->total_length;
                missile->x_current = (float)missile->x_origin + fraction * (float)(missile->x_dest - missile->x_origin);
                missile->y_current = (float)missile->y_origin + fraction * (float)(missile->y_dest - missile->y_origin);
                lcd_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, CONFIG_COLOR_ENEMY_MISSILE);
            } else if(missile->type == MISSILE_TYPE_PLAYER){
                missile->length += CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK;
                fraction = missile->length / missile->total_length;
                missile->x_current = (float)missile->x_origin + fraction * (float)(missile->x_dest - missile->x_origin);
                missile->y_current = (float)missile->y_origin + fraction * (float)(missile->y_dest - missile->y_origin);
                lcd_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, CONFIG_COLOR_PLAYER_MISSILE);
                if(missile->length >= missile->total_length){
                    missile->explode_me = true;
                }
            } else if(missile->type == MISSILE_TYPE_PLANE){
                missile->length += CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK;
                fraction = missile->length / missile->total_length;
                missile->x_current = (float)missile->x_origin + fraction * (float)(missile->x_dest - missile->x_origin);
                missile->y_current = (float)missile->y_origin + fraction * (float)(missile->y_dest - missile->y_origin);
                lcd_drawLine(missile->x_origin, missile->y_origin, missile->x_current, missile->y_current, CONFIG_COLOR_PLANE_MISSILE);
            }
            break;
        case(exploding_grow_st):
            missile->radius += CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;
            lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, CONFIG_COLOR_PLAYER_MISSILE);
            break;
        case(exploding_shrink_st):
            missile->radius -= CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;
            lcd_fillCircle(missile->x_current, missile->y_current, missile->radius, CONFIG_COLOR_PLAYER_MISSILE);
            break;
        case(impacted_st):
            break;
    }
}

/******************** Missile Status Functions ********************/

// Return the current missile position through the pointers *x,*y.
void missile_get_pos(missile_t *missile, coord_t *x, coord_t *y){
    *x = missile->x_current;
    *y = missile->y_current;
}

// Return the missile type.
missile_type_t missile_get_type(missile_t *missile){
    return missile-> type;
}

// Return whether the given missile is moving.
bool missile_is_moving(missile_t *missile){
    if(missile->currentState == moving_st){
        return true;
    }
    return false;
}

// Return whether the given missile is exploding. If this missile
// is exploding, it can explode another intersecting missile.
bool missile_is_exploding(missile_t *missile){
    if(missile->currentState == exploding_grow_st || missile->currentState == exploding_shrink_st){
        return true;
    }
    return false;
}

// Return whether the given missile is idle.
bool missile_is_idle(missile_t *missile){
    if(missile->currentState == idle_st){
        return true;
    }
    return false;
}

// Return whether the given missile is impacted.
bool missile_is_impacted(missile_t *missile){
    if(missile->type == MISSILE_TYPE_ENEMY || missile->type == MISSILE_TYPE_PLANE){
        if(missile->y_current >= LCD_H || missile->x_current >= LCD_W || missile->x_current <= 0){
            return true;
        }
    } else if(missile->type == MISSILE_TYPE_PLAYER){
        if(missile->y_current <= 0 || missile->x_current >= LCD_W || missile->x_current <= 0){
            return true;
        }
    } 
    return false;
}

// Return whether an object (e.g., missile or plane) at the specified
// (x,y) position is colliding with the given missile. For a collision
// to occur, the missile needs to be exploding and the specified
// position needs to be within the explosion radius.
bool missile_is_colliding(missile_t *missile, coord_t x, coord_t y){
    float dx = (float)x - (float)missile->x_current;
    float dy = (float)y - (float)missile->y_current;
    float dist = sqrtf(dy*dy + dx*dx);

    if(missile_is_exploding(missile) && (dist < missile->radius)){
        return true;
    }

    return false;

}