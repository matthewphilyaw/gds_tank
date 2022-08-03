#ifndef ROBOT_PLATFORM_PLATFORM_H
#define ROBOT_PLATFORM_PLATFORM_H

#include "gds_tank/drivers/motor_driver.h"

typedef enum { SUCCESS, ERROR = -1 } board_init_result_state_t;
typedef struct board_init_result_t {
    board_init_result_state_t state;
    motor_t left_motor;
    motor_t right_motor;
    uint motor_standby_gpio;
} board_init_result_t;

board_init_result_t board_init();

#endif //ROBOT_PLATFORM_PLATFORM_H
