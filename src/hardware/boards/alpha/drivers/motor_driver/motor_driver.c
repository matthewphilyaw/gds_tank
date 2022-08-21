#include "gds_tank/drivers/motor_driver.h"

#include "pico/stdlib.h"
#include "hardware/pwm.h"

motor_t motor_driver_init(motor_config_t config)
{
    gpio_put(config.clockwise_gpio, 0);
    gpio_put(config.counter_clockwise_gpio, 0);
    pwm_set_chan_level(config.pwm_slice, config.pwm_channel, config.pwm_min_level);

    motor_t motor = {
        .pwm_slice = config.pwm_slice,
        .pwm_channel = config.pwm_channel,
        .pwm_min_level = config.pwm_min_level,
        .pwm_max_level = config.pwm_max_level,
        .pwm_current_level = config.pwm_min_level,
        .clockwise_gpio = config.clockwise_gpio,
        .counter_clockwise_gpio = config.counter_clockwise_gpio
    };

    return motor;
}

void motor_driver_set_clockwise(motor_t *motor)
{
    gpio_put(motor->clockwise_gpio, 1);
    gpio_put(motor->counter_clockwise_gpio, 0);
}

void motor_driver_set_counter_clockwise(motor_t *motor)
{
    gpio_put(motor->clockwise_gpio, 0);
    gpio_put(motor->counter_clockwise_gpio, 1);
}

void motor_driver_short_brake(motor_t *motor)
{
    gpio_put(motor->clockwise_gpio, 1);
    gpio_put(motor->counter_clockwise_gpio, 1);
}

void motor_driver_stop(motor_t *motor)
{
    gpio_put(motor->clockwise_gpio, 0);
    gpio_put(motor->counter_clockwise_gpio, 0);
}

void motor_driver_set_power(motor_t *motor, uint16_t level)
{
    if (level < motor->pwm_min_level || level > motor->pwm_max_level)
    {
        // power level is not within range, slightly ignore for now
        return;
    }

    pwm_set_chan_level(motor->pwm_slice, motor->pwm_channel, level);
    motor->pwm_current_level = level;
}

void motor_driver_set_max_power(motor_t *motor)
{
    motor_driver_set_power(motor, motor->pwm_max_level);
}

void motor_driver_set_min_power(motor_t *motor)
{
    motor_driver_set_power(motor, motor->pwm_min_level);
}
