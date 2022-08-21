add_library(gds_alpha_board STATIC ${CMAKE_CURRENT_LIST_DIR}/alpha.c)

add_subdirectory(drivers)

target_link_libraries(gds_alpha_board
    PRIVATE pico_stdlib pico_cyw43_arch_lwip_poll hardware_pwm gds_alpha_motor_driver gds_alpha_network_driver gds_hardware
)