add_library(gds_core_comms STATIC ${CMAKE_CURRENT_LIST_DIR}/comms.c)

target_compile_definitions(gds_core_comms PRIVATE
        BASE_STATION_SERVER_IP=\"${BASE_STATION_SERVER_IP}\"
        )

target_link_libraries(gds_core_comms
    PRIVATE pico_stdlib pico_lwip gds_alpha_network_driver gds_hardware gds_core
)