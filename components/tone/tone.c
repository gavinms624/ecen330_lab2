#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "tone.h"

#define DOUBLE 2.0f
#define BIAS 0x80
#define ENUM_SIZE 5
#define PI 3.14159265358979323846f
#define MAX_AMP 255
#define MIN_AMP 0
#define ROUNDING_NUM 0.5f
#define THREE_QUARTER_SCALE 3.0f
#define QUARTER 4.0f
#define HALF 2.0f


uint8_t* buffer;
uint32_t sample_rate = 0;
float sample_num_period = 0;
uint32_t num_sample_points = 0;

int32_t tone_init(uint32_t sample_hz){
    if(sample_hz >= (DOUBLE*LOWEST_FREQ)){
        sample_rate = sample_hz;
        sound_init(sample_hz);
        num_sample_points = (sample_hz/LOWEST_FREQ + ROUNDING_NUM);
        buffer = malloc(num_sample_points * sizeof(uint8_t));
        return 0;
    }
    return 1;
}

int32_t tone_deinit(void){
    free(buffer);
    buffer = NULL;
    return sound_deinit();
}

void tone_start(tone_t tone, uint32_t freq){
    if(tone < ENUM_SIZE && freq >= LOWEST_FREQ){
        sample_num_period = (float)(sample_rate/freq);
        float three_quarter_period = ((sample_num_period/QUARTER) * THREE_QUARTER_SCALE);
        float half_period = (sample_num_period/HALF);
        float quarter_period = (sample_num_period/QUARTER);
        // Funcionlaity for selected tone
        switch(tone){
            case(SINE_T):
                float sin_input_res = (2*PI/sample_num_period);
                for(uint32_t i = 0; i <= sample_num_period; i++){
                    buffer[i] = (uint8_t)(((float)(BIAS-1)*(sinf((sin_input_res*i))) + (float)(BIAS)) + ROUNDING_NUM);
                }
                break;
            case(SQUARE_T):
                for(uint32_t i = 0; i < sample_num_period; i++){
                    if(i <= half_period){
                        buffer[i] = MAX_AMP;
                    }
                    else{
                        buffer[i] = MIN_AMP;
                    }
                }
                break;
            case(TRIANGLE_T):
                float slope = 0;
                for(uint32_t i = 0; i < sample_num_period; i++){
                    if(i <= quarter_period){
                        slope = ((BIAS - 1)/quarter_period);
                        buffer[i] = (slope * i + (BIAS));
                    } else if(i > quarter_period && i <= three_quarter_period){
                        slope = ((((MAX_AMP-1)-MIN_AMP))/half_period);
                        buffer[i] = ((-1)*slope * i + (MAX_AMP) + (BIAS));
                    } else if(i > three_quarter_period && i < sample_num_period){
                        slope = ((BIAS)/quarter_period);
                        buffer[i] = (slope * i - (MAX_AMP-1) + BIAS);
                    }
                }
                break;

            case(SAW_T):
                slope = 0;
                for(uint32_t i = 0; i < sample_num_period; i++){
                    if(i <= half_period){
                        slope = ((BIAS - 1)/half_period);
                        buffer[i] = (slope * i + (BIAS));
                    } else if(i > half_period && i < sample_num_period){
                        slope = ((BIAS-1)/half_period);
                        buffer[i] = (slope * i - (MAX_AMP-1) + BIAS);
                    }
                }
                break;
            case(LAST_T):
                break;
        }
        sound_cyclic(buffer, (sample_num_period * sizeof(uint8_t)));
    }

    
}