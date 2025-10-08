#include <stdlib.h>
#include <math.h>

#define DOUBLE 2
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
    if(tone < 5 && freq >= LOWEST_FREQ){
        uint32_t sample_num_period = (sample_rate/freq);
        switch(tone){
            case(SINE_T):
                break;
            case(SQUARE_T):
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