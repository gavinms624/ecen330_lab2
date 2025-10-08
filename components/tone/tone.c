#include <stdlib.h>
#include <math.h>

#define DOUBLE 2
#define BIAS 127
#define ENUM_SIZE 5
#define PI 3.1415f
#define MAX_AMP 255
#define MIN_AMP 0

uint32_t sample_rate;

int32_t tone_init(uint32_t sample_hz){
    if(sample_hz >= (DOUBLE*LOWEST_FREQ)){
        sample_rate = sample_hz;
        sound_init(uint32_t sample_hz);
        uint32_t num_sample_points = (sample_hz/LOWEST_FREQ);
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
        uint32_t sample_num_period = (sample_rate/freq);
        switch(tone){
            case(SINE_T):
                float sin_input_res = (2*pi/num_sample_points);
                for(uint8_t i = 0; i < sample_num_period; i++){
                    buffer[i] = ((MAX_AMP/2)*(float)(sinf((float)(sin_input_res*i) + (float)(pi/2))) + MAX_AMP/2);
                }
                break;
            case(SQUARE_T):
                uint32_t half_period = sample_num_period/2;
                for(uint8_t i = 0; i < sample_num_period; i++){
                    if(i <= half_period){
                        buffer[i] = MAX_AMP;
                    }
                    else{
                        buffer[i] = MIN_AMP;
                    }
                }
                break;
            case(TRIANGLE_T):
                break;
            case(SAW_T):
                break;
            case(LAST_T):
                break;
        }
    }
}