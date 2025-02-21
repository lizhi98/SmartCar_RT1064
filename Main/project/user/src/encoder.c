#include "encoder.h"

Encoder encoder_left = {
    .encoder_index      = QTIMER1_ENCODER1,
    .encoder_channel_1  = QTIMER1_ENCODER1_CH1_C0,
    .encoder_channel_2  = QTIMER1_ENCODER1_CH2_C1,
};
Encoder encoder_right = {
    .encoder_index      = QTIMER1_ENCODER2,
    .encoder_channel_1  = QTIMER1_ENCODER2_CH1_C2,
    .encoder_channel_2  = QTIMER1_ENCODER2_CH2_C24,
};
Encoder encoder_front = {
    .encoder_index      = QTIMER2_ENCODER1,
    .encoder_channel_1  = QTIMER2_ENCODER1_CH1_C3,
    .encoder_channel_2  = QTIMER2_ENCODER1_CH2_C25,
};

void encoder_all_init(){
    encoder_quad_init(encoder_left.encoder_index,encoder_left.encoder_channel_1,encoder_left.encoder_channel_2);
    encoder_quad_init(encoder_right.encoder_index,encoder_right.encoder_channel_1,encoder_right.encoder_channel_2);
    encoder_quad_init(encoder_front.encoder_index,encoder_front.encoder_channel_1,encoder_front.encoder_channel_2);
}
