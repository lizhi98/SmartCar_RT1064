// #include "motion_control.h"

// #define CUBE_RIGHT_CENTER_X  150
// #define CUBE_RIGHT_CENTER_Y  150

// vuint8 run_flag = 0;

// uint8 KI_clear_flag[5] = {0,0,0,0,0};

// LineOffset line_offset;

// Encoder encoder_left = {
//     .encoder_index      = QTIMER1_ENCODER1,
//     .encoder_channel_1  = QTIMER1_ENCODER1_CH1_C0,
//     .encoder_channel_2  = QTIMER1_ENCODER1_CH2_C1,
// };
// Encoder encoder_right = {
//     .encoder_index      = QTIMER1_ENCODER2, 
//     .encoder_channel_1  = QTIMER1_ENCODER2_CH1_C2,
//     .encoder_channel_2  = QTIMER1_ENCODER2_CH2_C24,
// };
// Encoder encoder_rear = {
//     .encoder_index      = QTIMER2_ENCODER1,
//     .encoder_channel_1  = QTIMER2_ENCODER1_CH1_C3,
//     .encoder_channel_2  = QTIMER2_ENCODER1_CH2_C4,
// };

// // 电机速度PID
// MotorPID motor_left_pid = {
//     .KP = 2.0,          .KI = 0.00,        .KD = 0.0,
//     .last_wrong = 0,    .sum_wrong = 0,    .wrong = 0,
// };
// MotorPID motor_right_pid = {
//     .KP = 2.0,          .KI = 0.00,        .KD = 0.0,
//     .last_wrong = 0,    .sum_wrong = 0,    .wrong = 0,
// };
// MotorPID motor_rear_pid = {
//     .KP = 2.0,          .KI = 0.00,        .KD = 0.0,
//     .last_wrong = 0,    .sum_wrong = 0,    .wrong = 0,
// };

// // 自转PID
// RotationPID rotation_pid = {
//     .normal_kp  = -2.9,           .normal_ki  = -0.03,          .normal_kd  = -0.3,
//  // .normal_kp = -2.6,           .normal_ki = -0.05,          .normal_kd = -0.5,
//     .curve_kp  = -3.0,           .curve_ki  = -0.03,          .curve_kd  = -0.2,
//     .last_offset = 0,            .sum_offset = 0,             .offset = 0,
// };

// // 平移PID
// TranslationPID   translation_pid = {
//     .front_KP = 4.0,        .front_KI = 0.01,       .front_KD = 0.8,
//     .front_sum_offset = 0,  .front_offset = 0,      .front_last_offset = 0,       .front_speed_out = 0,

//     .left_KP = 3.5,         .left_KI = 0.01,        .left_KD = 0.8,
//     .left_sum_offset = 0,   .left_offset = 0,       .left_last_offset = 0,        .left_speed_out = 0,
// };

// // TODO
// Motor motors[MOTOR_INDEX_MAX_PLUS_ONE] = {
//     {
//         .index = LEFT,              .pwm_channel_pin = PWM2_MODULE0_CHB_C7,   .gpio_dir_pin = C6,
//         .pwm_duty = 0,              .current_speed = 0,                       .set_speed = 0,
//         .encoder = &encoder_left,   .PID = &motor_left_pid,
//     },
//     {
//         .index = RIGHT,             .pwm_channel_pin = PWM2_MODULE3_CHB_D3,   .gpio_dir_pin = D2,
//         .pwm_duty = 0,              .current_speed = 0,                       .set_speed = 0,
//         .encoder = &encoder_right,  .PID = &motor_right_pid,
//     },
//     {
//         .index = REAR,              .pwm_channel_pin = PWM2_MODULE2_CHB_C11,  .gpio_dir_pin = C10,
//         .pwm_duty = 0,              .current_speed = 0,                       .set_speed = 0,
//         .encoder = &encoder_rear,   .PID = &motor_rear_pid,
//     },
// };

// // TARGET MOTION
// MotionControl motion_control = {
//     .motion_mode        = LINE_FOLLOW,  .last_motion_mode = LINE_FOLLOW,
//     .line_distance      = 0,            .line_gyro_angle_z  = 0.0f,
// };

// // 启动所有电机PWM通道输出，占空比为0
// void motor_all_init(void){
//     for(int i = 0; i < MOTOR_INDEX_MAX_PLUS_ONE; i++){
//         pwm_init(motors[i].pwm_channel_pin,     MOTOR_PWM_FREQUENCY,    0);
//         gpio_init(motors[i].gpio_dir_pin,       GPO,                    1,     GPO_PUSH_PULL);
//     }
// }

// // 设置电机PWM通道输出占空比
// void motor_set_duty(MotorIndex index, int32 duty){
//     // 占空比限幅
//     if(abs(duty) > MOTOR_PWM_DUTY_MAX){
//         (duty > 0) ? (duty = MOTOR_PWM_DUTY_MAX) : (duty = -MOTOR_PWM_DUTY_MAX);
//     }
//     if(duty > 0){
//         gpio_set_level(motors[index].gpio_dir_pin, 1);
//     }else if(duty < 0){
//         gpio_set_level(motors[index].gpio_dir_pin, 0);
//     }
//     pwm_set_duty(motors[index].pwm_channel_pin,    abs(duty));
//     // 记录占空比
//     motors[index].pwm_duty = duty;
// }

// void motor_plus_duty(MotorIndex index, int32 delta_duty){
//     motor_set_duty(index, motors[index].pwm_duty + delta_duty);
// }

// void motor_run_with_speed(MotorIndex index, int32 speed){
//     motors[index].set_speed = speed;
// }
// void motor_unpower(MotorIndex index){
//     motor_set_duty(index,0);
// }
// void motor_all_stop(void){
//     for(MotorIndex index = 0;index < MOTOR_INDEX_MAX_PLUS_ONE;index ++){
//         motors[index].set_speed = 0;
//     }
// }

// void motor_pid_calc_apply(MotorIndex index){
    
//     double out = 0.;
//     Motor * motor = &motors[index];
//     motor->PID->wrong = motor->set_speed - motor->current_speed;
//     // 比例
//     out += (double) (motor->PID->KP * motor->PID->wrong);

//     if(KI_clear_flag[index] >= 10){
//         motors[index].PID->sum_wrong = 0;
//         motor_set_duty(index,0);
//         KI_clear_flag[index] = 0;
//     }
    
//     if(motor->PID->wrong == 0 && KI_clear_flag[index] < 10){
//         KI_clear_flag[index]++;
//     }else{
//         KI_clear_flag[index] = 0;
//     }
//     // 积分
//     motor->PID->sum_wrong += motor->PID->wrong;
//     // 积分限幅
//     if(abs(motor->PID->sum_wrong) > SUM_WRONG_MAX){
//         (motor->PID->sum_wrong > 0) ? (motor->PID->sum_wrong = SUM_WRONG_MAX) : (motor->PID->sum_wrong = -SUM_WRONG_MAX);
//     }
//     out += (double) (motor->PID->KI * motor->PID->sum_wrong);

//     // 微分
//     out += (double) (motor->PID->KD * (motor->PID->wrong - motor->PID->last_wrong));

//     // 记录wrong
//     motor->PID->last_wrong = motor->PID->wrong;

//     // 输出
//     motor_plus_duty(index, (int32)out);
// }


// void motor_pid_pit_call(void){
//     for(MotorIndex index = 0;index < MOTOR_INDEX_MAX_PLUS_ONE;index ++){
//         motor_pid_calc_apply(index);
//     }
// }

// // Motor encoder
// void encoder_all_init(){
//     encoder_quad_init(encoder_left.encoder_index,   encoder_left.encoder_channel_1,     encoder_left.encoder_channel_2);
//     encoder_quad_init(encoder_right.encoder_index,  encoder_right.encoder_channel_1,    encoder_right.encoder_channel_2);
//     encoder_quad_init(encoder_rear.encoder_index,   encoder_rear.encoder_channel_1,     encoder_rear.encoder_channel_2);
// }
// void motor_encoder_pit_init(void){
//     pit_ms_init(MOTOR_ENCODER_PIT,MOTOR_ENCODER_PIT_TIME);
// }
// void motor_encoder_pit_call(void){
//     // 获取两个正交编码器的速度
//     for(MotorIndex index = 0;index < REAR;index ++){
//         // 获取速度
//         motors[index].current_speed = encoder_get_speed(motors[index].encoder->encoder_index);
//         // 清除计数
//         encoder_clear_count(motors[index].encoder->encoder_index);
//     }
//     // 获取后面方向编码器速度
//     motors[REAR].current_speed = -1 * encoder_get_speed(motors[REAR].encoder->encoder_index);
//     // 清除计数
//     encoder_clear_count(motors[REAR].encoder->encoder_index);

// }

// // ROTATION
// // 车体旋转PID计算
// void rotation_pid_calc(){
//     if(motion_control.motion_mode != LINE_FOLLOW ){
//         rotation_pid.wl_out = 0;
//         return;
//     }
//     if(run_flag == 0){
//         rotation_pid.offset = 0;
//         rotation_pid.wl_out = 0;
//         return;
//     }
//     double out = 0.;

//     // 获取offset
//     if(line_offset.times > 0){
//         rotation_pid.offset = line_offset.sum_offset / (double)line_offset.times;
//         line_offset.sum_offset = 0;
//         line_offset.times = 0;
//     }else{
//         rotation_pid.offset = image_result.offset;
//     }
//     // if(motion_control.motion_mode == CUBE_DISTANCE_POSITION){
//     //     rotation_pid.offset = (CUBE_RIGHT_CENTER_X - cube_info.x_center) * -0.2;
//     // }

//     float KP,KI,KD;
//     if(image_result.element_type == Normal || image_result.element_type == Cross || image_result.element_type == CrossBefore
//         || image_result.element_type == LoopLeftBefore || image_result.element_type == LoopRightBefore
//     ){
//         KP = rotation_pid.normal_kp;
//         KI = rotation_pid.normal_ki;
//         KD = rotation_pid.normal_kd;
//     }else{
//         KP = rotation_pid.curve_kp;
//         KI = rotation_pid.curve_ki;
//         KD = rotation_pid.curve_kd;
//     }
//     // if(motion_control.motion_mode == CUBE_DISTANCE_POSITION){
//     //     rotation_pid.offset = (CUBE_RIGHT_CENTER_X - cube_info.x_center) * -0.2;
//     // }
//     // 比例
//     out += (double) (KP * rotation_pid.offset);
//     // 积分
//     rotation_pid.sum_offset += rotation_pid.offset;
//     // 积分限幅
//     if(abs(rotation_pid.sum_offset) > ROTATION_SUM_OFFSET_MAX){
//         (rotation_pid.sum_offset > 0) ? (rotation_pid.sum_offset = ROTATION_SUM_OFFSET_MAX) : (rotation_pid.sum_offset = -ROTATION_SUM_OFFSET_MAX);
//     }
//     // 积分
//     out += (double) (KI * rotation_pid.sum_offset);
//     // 微分
//     out += (double) (KD * (rotation_pid.offset - rotation_pid.last_offset));

//     // 记录wrong
//     rotation_pid.last_offset = rotation_pid.offset;
//     // 限幅
//     if(out > 100.0){
//         rotation_pid.wl_out = 100;
//     }else if(out < -100.0){
//         rotation_pid.wl_out = -100;
//     }else{
//         rotation_pid.wl_out = (int32)out;
//     }

// }

// void translation_pid_calc(void){
//     // run_flag
//     if(!run_flag){
//         translation_pid.front_speed_out = 0;
//         translation_pid.left_speed_out  = 0;
//         return;
//     }

//     // 分情况获取offset，或者直接输出speed_out
//     switch (motion_control.motion_mode)
//     {
//     case LINE_FOLLOW:
//         // 巡线需要根据赛道元素决定速度
//         switch (image_result.element_type)
//         {
//         case Normal:
//         case Cross:
//         case CrossBefore:
//         case LoopLeftBefore:
//         case LoopRightBefore:
//             translation_pid.front_speed_out = MOTOR_NORMAL_SPEED;
//             break;
//         case Zebra:
//             // translation_pid.front_speed_out = 0;
//             break;
//         default:
//             translation_pid.front_speed_out = MOTOR_CURVE_SPEED;
//             break;
//         }
//         // translation_pid.front_speed_out = 200;
//         translation_pid.left_speed_out = 0;

//         // PID积分清零
//         translation_pid.front_sum_offset = 0;
//         translation_pid.left_sum_offset  = 0;
//         return;
//         break;
//     case CUBE_DISTANCE_POSITION:
//         translation_pid.left_offset  = (CUBE_RIGHT_CENTER_X - cube_info.x_center)/3.0;
//         translation_pid.front_offset =-1 * ( -CUBE_RIGHT_CENTER_Y + cube_info.y_center)/1.2;
//         break;
//     case CUBE_ANGLE_POSITION_LEFT:
//     case CUBE_ANGLE_POSITION_RIGHT:
//         translation_pid.front_speed_out = 0;
//         translation_pid.left_speed_out  = 0;
//         return;
//         break;
//     case CUBE_PUSH:
//         translation_pid.front_speed_out = 0;
//         translation_pid.left_speed_out  = 0;
//         return;
//         break;
//     case LINE_BACK:
//         translation_pid.front_speed_out = 0;
//         translation_pid.left_speed_out  = 0;
//         return;
//         break;
//     default:
//         translation_pid.left_speed_out  = 0;
//         translation_pid.front_speed_out = 0;
//         return;
//         break;
//     }
//     // 获取到了offset，进行PID计算
//     double left_speed_out = 0. ,front_speed_out = 0.;
//     // ===========================比例===========================
//     front_speed_out += (double) (translation_pid.front_KP   * translation_pid.front_offset);
//     left_speed_out  += (double) (translation_pid.left_KP    * translation_pid.left_offset);
//     // ===========================积分===========================
//     translation_pid.front_sum_offset += translation_pid.front_offset;
//     translation_pid.left_sum_offset  += translation_pid.left_offset;
//     // 积分清零
//     // if(translation_pid.front_offset == 0){
//     //     translation_pid.front_sum_offset = 0;
//     // }
//     // if(translation_pid.left_offset == 0){
//     //     translation_pid.left_sum_offset = 0;
//     // }
//     // 积分限幅
//     if(abs(translation_pid.front_sum_offset) > TRANSLATION_SUM_OFFSET_MAX){
//         (translation_pid.front_sum_offset > 0) ? (translation_pid.front_sum_offset = ROTATION_SUM_OFFSET_MAX) : (translation_pid.front_sum_offset = -ROTATION_SUM_OFFSET_MAX);
//     }
//     if(abs(translation_pid.left_sum_offset) > TRANSLATION_SUM_OFFSET_MAX){
//         (translation_pid.left_sum_offset > 0) ? (translation_pid.left_sum_offset = ROTATION_SUM_OFFSET_MAX) : (translation_pid.left_sum_offset = -ROTATION_SUM_OFFSET_MAX);
//     }
//     // 积分计算
//     front_speed_out += (double) (translation_pid.front_KI * translation_pid.front_sum_offset);
//     left_speed_out  += (double) (translation_pid.left_KI  * translation_pid.left_sum_offset);

//     // ===========================微分===========================
//     front_speed_out += (double) (translation_pid.front_KD * (translation_pid.front_offset - translation_pid.front_last_offset));
//     left_speed_out  += (double) (translation_pid.left_KD  * (translation_pid.left_offset - translation_pid.left_last_offset));

//     // 记录offset
//     translation_pid.front_last_offset = translation_pid.front_offset;
//     translation_pid.left_last_offset  = translation_pid.left_offset;

//     // 输出
//     translation_pid.front_speed_out = (int32)front_speed_out;
//     translation_pid.left_speed_out  = (int32)left_speed_out;
// }

// void motion_pid_pit_init(void){
//     pit_ms_init(MOTION_PID_PIT,MOTION_PID_PIT_TIME);
// }

// void motion_pid_callback(void){
    
//     translation_pid_calc();
//     rotation_pid_calc();
//     target_speed_calc();
// }

// uint8 cube_distance_position_ok(){
//     // 立方体坐标定位是否正确
//     if( (abs(cube_info.x_center - CUBE_RIGHT_CENTER_X) <= 15) && ( abs(cube_info.y_center - CUBE_RIGHT_CENTER_Y) <= 15 )){
//         return 1u;
//     }else{
//         return 0u;
//     }
// }

// uint8 cube_angle_position_ok(){
//     static uint8 right_count = 0;
//     // 立方体坐标定位是否正确
//     if(abs(rotation_pid.offset) <= 10 ){
//         right_count++;
//         if (right_count >= 10){
//             right_count = 0;
//             return 1u;
//         }
//     }
//     return 0u;
// }

// uint8 cube_push_ok(){
//     static uint8 push_count = 0;
//     // 立方体坐标定位是否正确
    
//     if(abs(translation_pid.front_offset) <= 100 ){
//         push_count++;
//         if (push_count >= 10){
//             push_count = 0;
//             return 1u;
//         }
//     }
    
//     return 0u;
    
// }

// uint8 line_back_ok(){
//     // 立方体坐标定位是否正确
//     static uint8 back_count = 0;
//     // 立方体坐标定位是否正确
    
//     if(abs(translation_pid.front_offset) <= 100 ){
//         back_count++;
//         if (back_count >= 10){
//             back_count = 0;
//             return 1u;
//         }
//     }
    
//     return 0u;
// }

// uint32 push_box_valid_time = 0;
// // 车运动解算函数
// void target_motion_calc(void){
//     // motion_control.motion_mode = CUBE_DISTANCE_POSITION;
//     // motion_control.motion_mode = LINE_FOLLOW;
//     if(!run_flag){
//         system_delay_ms(500);
//         motor_run_with_speed(LEFT,0);
//         motor_run_with_speed(RIGHT,0);
//         motor_run_with_speed(REAR,0);
//         while(1){}
//         return;
//     }
    
//     if(motion_control.motion_mode == LINE_FOLLOW && cube_info.state == CUBE_INSIDE_VIEW){
//         motion_control.motion_mode = CUBE_DISTANCE_POSITION;
//         while(1){
//             if(cube_info.state == CUBE_OUTSIDE_VIEW){
//                 motion_control.motion_mode = LINE_FOLLOW;
//                 return;
//             }
//         }
        
// //         while(!cube_distance_position_ok()){
// //             if(cube_info.state == CUBE_OUTSIDE_VIEW){
// //                 motion_control.motion_mode = LINE_FOLLOW;
// //                 return;
// //             }
// //         }
// //         // 先停车
// //         motor_run_with_speed(LEFT,0);
// //         motor_run_with_speed(RIGHT,0);
// //         motor_run_with_speed(REAR,0);
// // //        system_delay_ms(500);
// //         push_box();
//         motion_control.motion_mode = LINE_FOLLOW;
//         return;
//     }

//     // target_speed_calc();
// }

// void target_speed_calc(){
    
//     if(motion_control.motion_mode != CUBE_DISTANCE_POSITION && (motion_control.motion_mode != LINE_FOLLOW || run_flag == 0)){
//         return;
//     }
//     if(motion_control.motion_mode == CUBE_PUSH || motion_control.motion_mode == LINE_BACK){
//         return;
//     }

//     // 初始化三个电机速度缓存
//     int32   motor_left_speed    = 0,
//             motor_right_speed   = 0,
//             motor_rear_speed    = 0;
    
//     motor_left_speed    =   (int32) (-translation_pid.front_speed_out * COS_PI_D_6 + translation_pid.left_speed_out * SIN_PI_D_6);
//     motor_right_speed   =   (int32) ( translation_pid.front_speed_out * COS_PI_D_6 + translation_pid.left_speed_out * SIN_PI_D_6);
//     motor_rear_speed    =   (int32) (-translation_pid.left_speed_out);

//     motor_left_speed    +=  rotation_pid.wl_out;
//     motor_right_speed   +=  rotation_pid.wl_out;
//     motor_rear_speed    +=  rotation_pid.wl_out;
    
//     // 应用速度
//     motor_run_with_speed(LEFT,motor_left_speed);
//     motor_run_with_speed(RIGHT,motor_right_speed);
//     motor_run_with_speed(REAR,motor_rear_speed);

//     // 记录运动模式
//     motion_control.last_motion_mode = motion_control.motion_mode;
// }
