#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lcd.h"
#include "pac.h"
#include "hw.h"

static const char *TAG = "lab01";

#define DELAY_MS(ms) \
	vTaskDelay(((ms)+(portTICK_PERIOD_MS-1))/portTICK_PERIOD_MS)

//----------------------------------------------------------------------------//
// Car Implementation - Begin
//----------------------------------------------------------------------------//

// Car constants
#define CAR_CLR rgb565(220,30,0)
#define WINDOW_CLR rgb565(180,210,238)
#define TIRE_CLR BLACK
#define HUB_CLR GRAY

// TODO: Finish car part constants
#define CAR_W 60
#define CAR_H 32
#define TOP_W 39
#define TOP_H 11

#define BODY_X0 0
#define BODY_Y0 12
#define BODY_X1 59
#define BODY_Y1 24

#define HOOD_X0 40
#define HOOD_Y0 11
#define HOOD_X1 40
#define HOOD_Y1 9
#define HOOD_X2 59
#define HOOD_Y2 11

#define TIRE_BACK_XC 11
#define TIRE_BACK_YC 24
#define TIRE_FRONT_XC 48
#define TIRE_FRONT_YC 24
#define TIRE_R 7
#define HUB_R 4

#define B_WINDOW_X0 3
#define B_WINDOW_Y0 1
#define B_WINDOW_X1 18
#define B_WINDOW_Y1 8
#define WINDOW_R 2
#define F_WINDOW_X0 21
#define F_WINDOW_Y0 1
#define F_WINDOW_X1 37
#define F_WINDOW_Y1 8

/**
 * @brief Draw a car at the specified location.
 * @param x      Top left corner X coordinate.
 * @param y      Top left corner Y coordinate.
 * @details Draw the car components relative to the anchor point (top, left).
 */
void drawCar(coord_t x, coord_t y)
{
	lcd_fillRect2(x+1, y, x + TOP_W, y + TOP_H, CAR_CLR); // Car Top
	lcd_fillRect2(x + BODY_X0, y + BODY_Y0, x + BODY_X1, y + BODY_Y1, CAR_CLR); // Car Body
	lcd_fillTriangle(x + HOOD_X0, y + HOOD_Y0, x + HOOD_X1, y + HOOD_Y1, x + HOOD_X2, y + HOOD_Y2, CAR_CLR); // Car Hood
	lcd_fillCircle(x + TIRE_BACK_XC, y + TIRE_BACK_YC, TIRE_R, TIRE_CLR); // Back Tire
	lcd_fillCircle(x + TIRE_BACK_XC, y + TIRE_BACK_YC, HUB_R, HUB_CLR); // Back Hub
	lcd_fillCircle(x + TIRE_FRONT_XC, y + TIRE_FRONT_YC, TIRE_R, TIRE_CLR); // Back Tire
	lcd_fillCircle(x + TIRE_FRONT_XC, y + TIRE_FRONT_YC, HUB_R, HUB_CLR); // Back Hub
	lcd_fillRoundRect2(x + B_WINDOW_X0, y + B_WINDOW_Y0, x + B_WINDOW_X1, y + B_WINDOW_Y1, WINDOW_R, WINDOW_CLR); // Back Window
	lcd_fillRoundRect2(x + F_WINDOW_X0, y + F_WINDOW_Y0, x + F_WINDOW_X1, y + F_WINDOW_Y1, WINDOW_R, WINDOW_CLR); // Front Window
}

//----------------------------------------------------------------------------//
// Car Implementation - End
//----------------------------------------------------------------------------//

// Main display constants
#define BACKGROUND_CLR rgb565(0,60,90)
#define TITLE_CLR GREEN
#define STATUS_CLR WHITE
#define STR_BUF_LEN 12 // string buffer length
#define FONT_SIZE 2
#define FONT_W (LCD_CHAR_W*FONT_SIZE)
#define FONT_H (LCD_CHAR_H*FONT_SIZE)
#define STATUS_W (FONT_W*3)

#define WAIT 2000 // milliseconds
#define DELAY_EX3 20 // milliseconds

// Object position and movement
#define OBJ_X 100
#define OBJ_Y 100
#define OBJ_MOVE 3 // pixels


// Application main
void app_main(void)
{
	ESP_LOGI(TAG, "Start up");
	lcd_init();
	lcd_fillScreen(BACKGROUND_CLR);
	lcd_setFontSize(FONT_SIZE);
	lcd_drawString(0, 0, "Hello World! (lcd)", TITLE_CLR);
	printf("Hello World! (terminal)\n");
	DELAY_MS(WAIT);

	// TODO: Exercise 1 - Draw car in one location.
	lcd_fillScreen(BACKGROUND_CLR);
	lcd_drawString(0, 0, "Exercise 1", TITLE_CLR);
	drawCar(OBJ_X, OBJ_Y);
	DELAY_MS(2000);

	// TODO: Exercise 2 - Draw moving car (Method 1), one pass across display.
	// Clear the entire display and redraw all objects each iteration.
	// Use a loop and increment x by OBJ_MOVE each iteration.
	// Start x off screen (negative coordinate).

	char x_buff[4];
	for (coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE) {
		lcd_fillScreen(BACKGROUND_CLR);
		lcd_drawString(0, 0, "Exercise 2", TITLE_CLR);
		drawCar(x, OBJ_Y);
		sprintf(x_buff, "%3ld", x);
		lcd_drawString(0, (LCD_H-FONT_H), x_buff, TITLE_CLR);
	}

	// TODO: Exercise 3 - Draw moving car (Method 2), one pass across display.
	// Move by erasing car at old position, then redrawing at new position.
	// Objects that don't change or move are drawn once.

	char x_buff2[4];
	lcd_fillScreen(BACKGROUND_CLR);
	lcd_drawString(0, 0, "Exercise 3", TITLE_CLR);
	for (coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE) {
		lcd_fillRect2(x, OBJ_Y, x + CAR_W, OBJ_Y + CAR_W, BACKGROUND_CLR);
		drawCar(x + OBJ_MOVE, OBJ_Y);
		sprintf(x_buff2, "%3ld", x);
		lcd_fillRect2(0, (LCD_H-FONT_H), FONT_W * 3, LCD_H, BACKGROUND_CLR);
		lcd_drawString(0, (LCD_H-FONT_H), x_buff2, TITLE_CLR);
		DELAY_MS(20);
	}

	// TODO: Exercise 4 - Draw moving car (Method 3), one pass across display.
	// First, draw all objects into a cleared, off-screen frame buffer.
	// Then, transfer the entire frame buffer to the screen.

	char x_buff3[4];
	lcd_frameEnable ();
	for (coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE) {
		lcd_fillScreen(BACKGROUND_CLR);
		lcd_drawString(0, 0, "Exercise 4", TITLE_CLR);
		drawCar(x, OBJ_Y);
		sprintf(x_buff3, "%3ld", x);
		lcd_drawString(0, (LCD_H-FONT_H), x_buff3, STATUS_CLR);
		lcd_writeFrame ();
	}

	// TODO: Exercise 5 - Draw an animated Pac-Man moving across the display.
	// Use Pac-Man sprites instead of the car object.
	// Cycle through each sprite when moving the Pac-Man character.
	char x_buff4[4];
	lcd_frameEnable ();
	uint16_t i = 0;
	const uint8_t pidx[] = {0, 1, 2, 1};
	for(;;){
		for (coord_t x = -CAR_W; x <= LCD_W; x += OBJ_MOVE) {
			lcd_fillScreen(BACKGROUND_CLR);
			lcd_drawString(0, 0, "Exercise 5", TITLE_CLR);
			lcd_drawBitmap(x, OBJ_Y, pac[pidx[i++ % sizeof(pidx)]], PAC_W, PAC_H, YELLOW);
			sprintf(x_buff4, "%3ld", x);
			lcd_drawString(0, (LCD_H-FONT_H), x_buff4, STATUS_CLR);
			lcd_writeFrame ();
		}
	}
}
