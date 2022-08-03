add_library(gds_alpha_board STATIC ${CMAKE_CURRENT_LIST_DIR}/alpha.c)

add_subdirectory(drivers)

target_link_libraries(gds_alpha_board
    PRIVATE pico_stdlib pico_cyw43_arch_none hardware_pwm gds_alpha_motor_driver gds_hardware
)