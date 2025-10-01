#include <stdio.h>

#include "driver/gptimer.h"
#include "hw.h"
#include "lcd.h"
#include "pin.h"
#include "watch.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "lab03";
volatile uint64_t timer_ticks;
volatile bool running;
int64_t start_cb, finish_cb;
volatile int64_t isr_max; // Maximum ISR execution time (us)
volatile int32_t isr_cnt; // Count of ISR invocations



static bool timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    isr_cnt++;
    start_cb = esp_timer_get_time();
    if(!pin_get_level(HW_BTN_A)){
        running = true;
    }
    if(!pin_get_level(HW_BTN_B)){
        running = false;
    }
    if(!pin_get_level(HW_BTN_START)){
        running = false;
        timer_ticks = 0;
    }
    if(running){
        timer_ticks++;
    }
    finish_cb = esp_timer_get_time();
    if(finish_cb-start_cb > isr_max){
        isr_max = finish_cb - start_cb;
    }
    return false;
}

// Main application
void app_main(void)
{   
    int64_t start, finish;
    start = esp_timer_get_time();
    //Initialize program and pins
    pin_reset(HW_BTN_A);
    pin_reset(HW_BTN_B);
    pin_reset(HW_BTN_START);
    pin_input(HW_BTN_A, 1);
    pin_input(HW_BTN_B, 1);
    pin_input(HW_BTN_START, 1);
    finish = esp_timer_get_time();
    printf("I/O Config time: %lld microseconds\n", finish-start);

    start = esp_timer_get_time();
    // Create a timer instance
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT, // Select the default clock source
        .direction = GPTIMER_COUNT_UP,      // Counting direction is up
        .resolution_hz = 1 * 1000 * 1000,   // Resolution is 1 MHz, i.e., 1 tick equals 1 microsecond
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    // Register timer event callback functions, allowing user context to be carried
    gptimer_event_callbacks_t cbs = {
    .on_alarm = timer_callback, // Call the user callback function when the alarm event occurs
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    gptimer_alarm_config_t alarm_config = {
    .reload_count = 0,      // When the alarm event occurs, the timer will automatically reload to 0
    .alarm_count = 10000, // Set the actual alarm period, since the resolution is 1us, 1000000 represents 1s
    .flags.auto_reload_on_alarm = true, // Enable auto-reload function
    };
    // Set the timer's alarm action
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    // Enable the timer
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    // Start the timer
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    finish = esp_timer_get_time();
    printf("Stopwatch config time: %lld microseconds\n", finish-start);


    // Display
    start = esp_timer_get_time();
    ESP_LOGI(TAG, "Stopwatch update");
    finish = esp_timer_get_time();
    printf("ESP_LOGI time: %lld microseconds\n", finish-start);
    lcd_init(); // Initialize LCD display
    watch_init(); // Initialize stopwatch face
    
    for (;;) { // forever update loop
        watch_update(timer_ticks);
        if(isr_cnt >= 500){
            printf("ISR Max time: %lld microseconds\n", isr_max);
            isr_max = 0;
            isr_cnt = 0;
        }
    }
}
