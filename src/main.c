#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "gds_tank/boards/board.h"
#include "gds_tank/drivers/motor_driver.h"
#include "gds_tank/comms/comms.h"

static board_init_result_t board_init_result;

static void control_loop()
{
    printf("control loop active.\n");

    motor_driver_set_min_power(&board_init_result.left_motor);
    motor_driver_set_min_power(&board_init_result.right_motor);


    uint_fast32_t state = 0;
    uint_fast32_t power_state = 1;
    while(1)
    {
        // forward
        if (state == 0)
        {
            gpio_put(board_init_result.motor_standby_gpio, true);
            motor_driver_set_clockwise(&board_init_result.left_motor);
            motor_driver_set_clockwise(&board_init_result.right_motor);
            state = 1;
        }
            // back
        else if (state == 1)
        {
            motor_driver_set_counter_clockwise(&board_init_result.left_motor);
            motor_driver_set_counter_clockwise(&board_init_result.right_motor);
            state = 2;
        }
            // turn right
        else if (state == 2)
        {
            motor_driver_set_counter_clockwise(&board_init_result.left_motor);
            motor_driver_set_clockwise(&board_init_result.right_motor);
            state = 3;
        }
            // turn left
        else if (state == 3)
        {
            motor_driver_set_clockwise(&board_init_result.left_motor);
            motor_driver_set_counter_clockwise(&board_init_result.right_motor);
            state = 4;
        }
            // stop right
        else if (state == 4)
        {
            motor_driver_set_clockwise(&board_init_result.left_motor);
            motor_driver_short_brake(&board_init_result.right_motor);
            state = 5;
        }
            // stop left
        else if (state == 5)
        {
            motor_driver_set_clockwise(&board_init_result.right_motor);
            motor_driver_short_brake(&board_init_result.left_motor);
            state = 6;
        }
            // stop both
        else if (state == 6)
        {
            motor_driver_short_brake(&board_init_result.right_motor);
            motor_driver_short_brake(&board_init_result.right_motor);
            state = 7;
        }
            // standby mode
        else
        {
            gpio_put(board_init_result.motor_standby_gpio, false);

            motor_driver_set_clockwise(&board_init_result.left_motor);
            motor_driver_set_clockwise(&board_init_result.right_motor);

            if (power_state == 0) {
                motor_driver_set_min_power(&board_init_result.left_motor);
                motor_driver_set_min_power(&board_init_result.right_motor);
                power_state = 1;
            } else {
                motor_driver_set_max_power(&board_init_result.left_motor);
                motor_driver_set_max_power(&board_init_result.right_motor);
                power_state = 0;
            }
            state = 0;
        }

        sleep_ms(3000);
    }
}

int main()
{
    board_init_result = board_init();

    if (board_init_result.state != SUCCESS) {
        return -1;
    };

    if (!comms_init()) {
        return -1;
    }

    printf("Launching control loop on second core.\n");
    multicore_launch_core1(control_loop);

    for(;;)
    {
        comms_poll();
        sleep_ms(1);
    }

    comms_deinit();
    return 0;
}