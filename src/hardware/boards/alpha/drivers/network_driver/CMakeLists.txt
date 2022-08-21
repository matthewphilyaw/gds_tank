add_library(gds_alpha_network_driver STATIC ${CMAKE_CURRENT_LIST_DIR}/network_driver.c)

target_compile_definitions(gds_alpha_network_driver PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
)

target_link_libraries(gds_alpha_network_driver PRIVATE pico_stdlib gds_hardware pico_cyw43_arch_lwip_poll)
