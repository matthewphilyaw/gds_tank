#ifndef ROBOT_PLATFORM_MOTOR_DRIVER_H
#define ROBOT_PLATFORM_MOTOR_DRIVER_H

#include "pico.h"

typedef struct motor_config_t {
    uint pwm_slice;
    uint pwm_channel;
    uint pwm_min_level;
    uint pwm_max_level;
    uint clockwise_gpio;
    uint counter_clockwise_gpio;
} motor_config_t;

typedef struct motor_t {
    uint pwm_slice;
    uint pwm_channel;
    uint pwm_min_level;
    uint pwm_max_level;
    uint16_t pwm_current_level;
    uint clockwise_gpio;
    uint counter_clockwise_gpio;
} motor_t;

motor_t motor_driver_init(motor_config_t config);
void motor_driver_set_clockwise(motor_t *motor);
void motor_driver_set_counter_clockwise(motor_t *motor);
void motor_driver_short_brake(motor_t *motor);
void motor_driver_stop(motor_t *motor);
void motor_driver_set_power(motor_t *motor, uint16_t level);
void motor_driver_set_max_power(motor_t *motor);
void motor_driver_set_min_power(motor_t *motor);

#endif //ROBOT_PLATFORM_MOTOR_DRIVER_H