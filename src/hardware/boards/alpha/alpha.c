#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/pwm.h"
#include "gds_tank/boards/board.h"

#define PWM_CLK_WRAP 16384
#define PWM_CLK_DIV 125

#define MOTOR_L_GPIO 7
#define MOTOR_L_PWM_CHANNEL PWM_CHAN_B
#define MOTOR_L_IN_1_GPIO 10
#define MOTOR_L_IN_2_GPIO 11

#define MOTOR_R_GPIO 6
#define MOTOR_R_PWM_CHANNEL PWM_CHAN_A
#define MOTOR_R_IN_1_GPIO 8
#define MOTOR_R_IN_2_GPIO 9

#define MOTOR_STANDBY_GPIO 12
#define MOTOR_CTRL_PIN_MASK 0x1f00

// Max power is defined as the top of the PWM CLK
#define MOTOR_MAX_POWER PWM_CLK_WRAP
// Min power for these motors will be half the max for usable range
#define MOTOR_MIN_POWER MOTOR_MAX_POWER / 2



board_init_result_t board_init()
{
    set_sys_clock_pll(756 * MHZ, 6, 1);
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return (board_init_result_t) {
            .state = ERROR
        };
    }

    // set GPIO for motor driver board
    gpio_init_mask(MOTOR_CTRL_PIN_MASK);
    gpio_set_dir_out_masked(MOTOR_CTRL_PIN_MASK);

    // pull the standby pin high
    gpio_put(MOTOR_STANDBY_GPIO, true);

    // set up PWM
    gpio_set_function(MOTOR_L_GPIO, GPIO_FUNC_PWM);
    gpio_set_function(MOTOR_R_GPIO, GPIO_FUNC_PWM);

    // This will likely be the same slice
    uint pwm_slice_motor_l = pwm_gpio_to_slice_num(MOTOR_L_GPIO);
    uint pwm_slice_motor_r = pwm_gpio_to_slice_num(MOTOR_R_GPIO);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv_int(&config, PWM_CLK_DIV);
    pwm_config_set_wrap(&config, PWM_CLK_WRAP);
    pwm_config_set_clkdiv_mode(&config, PWM_DIV_FREE_RUNNING);

    pwm_init(pwm_slice_motor_l, &config, true);
    pwm_init(pwm_slice_motor_r, &config, true);

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    motor_t left_motor = motor_driver_init((motor_config_t){
        .pwm_slice = pwm_slice_motor_l,
        .pwm_channel = MOTOR_L_PWM_CHANNEL,
        .clockwise_gpio = MOTOR_L_IN_1_GPIO,
        .counter_clockwise_gpio = MOTOR_L_IN_2_GPIO,
        .pwm_min_level = MOTOR_MIN_POWER,
        .pwm_max_level = MOTOR_MAX_POWER
    });

    motor_t right_motor = motor_driver_init((motor_config_t){
        .pwm_slice = pwm_slice_motor_r,
        .pwm_channel = MOTOR_R_PWM_CHANNEL,
        .clockwise_gpio = MOTOR_R_IN_1_GPIO,
        .counter_clockwise_gpio = MOTOR_R_IN_2_GPIO,
        .pwm_min_level = MOTOR_MIN_POWER,
        .pwm_max_level = MOTOR_MAX_POWER
    });

    return (board_init_result_t) {
        .state = SUCCESS,
        .left_motor = left_motor,
        .right_motor = right_motor,
        .motor_standby_gpio = MOTOR_STANDBY_GPIO
    };
}