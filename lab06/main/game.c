
#include <stdio.h>
#include <stdlib.h> // rand

#include "hw.h"
#include "lcd.h"
#include "cursor.h"
#include "sound.h"
#include "pin.h"
#include "missile.h"
#include "plane.h"
#include "game.h"
#include "config.h"

// sound support
#include "missileLaunch.h"

// M2: Define stats constants
#define IMPACTED_HEIGHT 10
#define VOL 0

// All missiles
missile_t missiles[CONFIG_MAX_TOTAL_MISSILES];

// Alias into missiles array
missile_t *enemy_missiles = missiles+0;
missile_t *player_missiles = missiles+CONFIG_MAX_ENEMY_MISSILES;
missile_t *plane_missile = missiles+CONFIG_MAX_ENEMY_MISSILES+
									CONFIG_MAX_PLAYER_MISSILES;

// M2: Declare stats variables
uint32_t shots;
uint32_t impacted;
coord_t x;
coord_t y;
coord_t p_x;
coord_t p_y;
char shots_buffer[20];
char impacted_buffer[20];

// Initialize the game control logic.
// This function initializes all missiles, planes, stats, etc.
void game_init(void)
{
	// Initialize missiles
	for (uint32_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++)
		missile_init(missiles+i);

	// Initialize plane
	plane_init(plane_missile);

	// M2: Initialize stats
	shots = 0;
	impacted = 0;

	// M2: Set sound volume
	sound_init(MISSILELAUNCH_SAMPLE_RATE);
	sound_set_volume(VOL);
	
}

// Update the game control logic.
// This function calls the missile & plane tick functions, relaunches
// idle enemy missiles, handles button presses, launches player missiles,
// detects collisions, and updates statistics.
void game_tick(void)
{
	// Tick missiles in one batch
	for (uint32_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++){
		missile_tick(missiles+i);
	}

	// Tick plane
	plane_tick();

	// Relaunch idle enemy missiles
	for (uint32_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++){
		if (missile_is_idle(enemy_missiles+i))
			missile_launch_enemy(enemy_missiles+i);
	}

	// M2: Check for button press. If so, launch a free player missile.
	// This code will indicate when any
	// button in HW_BTN_MASK is pressed and
	// also indicate when all buttons in
	// the mask are released. HW_BTN_MASK
	// is defined in "hw.h".
	static bool pressed = false;
	coord_t x, y;
	uint64_t btns;
	btns = ~pin_get_in_reg() & HW_BTN_MASK;
	if (!pressed && btns) {
		pressed = true; // button pressed
		cursor_get_pos(&x, &y);
		for (uint32_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++){
			if (missile_is_idle(player_missiles+i)){
				missile_launch_player(player_missiles+i, x, y);
				shots++;
				sound_start(missileLaunch, sizeof(missileLaunch), false);
				break;
			}
		}
		
	} else if (pressed && !btns) {
		pressed = false; // all released
	}



	// M2: Check for moving non-player missile collision with an explosion.
	for(uint32_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++){
		// Check Enemy Missiles
		for(uint32_t j = 0; j < CONFIG_MAX_ENEMY_MISSILES; j++){
			missile_get_pos(enemy_missiles+j, &x, &y);
			if(missile_is_colliding(player_missiles+i, x, y)){
				missile_explode(enemy_missiles+j);
			}
		}
		// Check Plane Missiles
		for(uint32_t j = 0; j < CONFIG_MAX_PLANE_MISSILES; j++){
			missile_get_pos(plane_missile+j, &x, &y);
			if(missile_is_colliding(player_missiles+i, x, y)){
				missile_explode(plane_missile+j);
			}
		}

		// M2: Check for flying plane collision with an explosion.
		plane_get_pos(&p_x, &p_y);
		if(plane_is_flying() && (missile_is_colliding(player_missiles+i, p_x, p_y) || missile_is_colliding(player_missiles+i, (p_x - CONFIG_PLANE_WIDTH), p_y))){
			plane_explode();
		}
	}


	// M2: Count non-player impacted missiles
	for(uint32_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++){
		if((enemy_missiles+i)->currentState == impacted_st){
			impacted++;
		}
	}
	if(plane_missile->currentState == impacted_st){
			impacted++;
	}
	

	// M2: Draw stats
	sprintf(shots_buffer, "Shots: %ld", shots);
	sprintf(impacted_buffer, "Impacted: %ld", impacted);
	lcd_drawString(0, 0, shots_buffer, CONFIG_COLOR_STATUS);
	lcd_drawString(0, IMPACTED_HEIGHT, impacted_buffer, CONFIG_COLOR_STATUS);
}
