add_library(gds_core_drive_system STATIC ${CMAKE_CURRENT_LIST_DIR}/drive_system.c)

target_link_libraries(gds_core_drive_system
    PRIVATE pico_stdlib gds_alpha_motor_driver gds_hardware gds_core
)