#ifndef _CODE_ENCODER_H_
#define _CODE_ENCODER_H_

#include "zf_driver_encoder.h"
#include "zf_driver_pit.h"

#define ENCODER_PIT         PIT_CH1
#define ENCODER_PIT_TIME    20

typedef struct _Encoder
{
    const encoder_index_enum    encoder_index;
    const encoder_channel1_enum encoder_channel_1;
    const encoder_channel2_enum encoder_channel_2;
}Encoder;

extern Encoder encoder_left;
extern Encoder encoder_right;
extern Encoder encoder_front;

void encoder_all_init(void);

#define encoder_get_speed(encoder_index) encoder_get_count(index);

#endif