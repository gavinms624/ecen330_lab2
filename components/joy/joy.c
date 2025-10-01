#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define MEAN_NUM 10000

uint32_t joy_center_6;
uint32_t joy_center_7;
adc_oneshot_unit_handle_t adc1_handle;

// Initialize Joystick, returns void
int32_t joy_init(){
    // Configure ADC one-shot unit
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //Configure ADC1 Channels 6 and 7
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_7, &config));

    // Find Center of Joystick
    int_fast32_t read;
    uint32_t count = 1;
    uint32_t sum = 0;
    while(count <= MEAN_NUM){
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &read);
        sum += read;
        read = 0;
        count++;
    }
    joy_center_6 = (sum/count);

    count = 1;
    sum = 0;
    while(count <= MEAN_NUM){
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_7, &read);
        sum += read;
        read = 0;
        count++;
    }
    joy_center_7 = (sum/count);
    return 0;
}

int32_t joy_deinit(){
    if(adc1_handle != NULL){
        adc_oneshot_del_unit(adc1_handle);
    }
    return 0;
}

void joy_get_displacement(int32_t *dcx, int32_t *dcy){
    int_fast32_t read_x;
    int_fast32_t read_y;
    adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &read_x);
    adc_oneshot_read(adc1_handle, ADC_CHANNEL_7, &read_y);
    *dcx = (read_x - joy_center_6);
    *dcy = (read_y - joy_center_7);
}
