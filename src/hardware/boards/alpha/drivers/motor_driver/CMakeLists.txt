add_library(gds_alpha_motor_driver STATIC ${CMAKE_CURRENT_LIST_DIR}/motor_driver.c)

target_link_libraries(gds_alpha_motor_driver PRIVATE pico_stdlib hardware_pwm gds_hardware)
