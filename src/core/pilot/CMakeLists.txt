add_library(gds_core_pilot STATIC ${CMAKE_CURRENT_LIST_DIR}/pilot.c)

target_link_libraries(gds_core_pilot
    PRIVATE pico_stdlib gds_alpha_motor_driver gds_hardware gds_core
)