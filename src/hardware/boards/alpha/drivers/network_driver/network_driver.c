#include "gds_tank/drivers/network_driver.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#define WIFI_CONNECT_RETRY_COUNT 3

uint network_driver_init()
{
    bool wifi_connected = false;
    for (uint retry_count = 0; retry_count < WIFI_CONNECT_RETRY_COUNT; retry_count++)
    {
        printf("Connecting to wifi - attempt: %d.\n", retry_count);

        if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
        {
            printf("failed to connect.\n");
        }
        else {
            wifi_connected = true;
            break;
        }
    }

    if (!wifi_connected)
    {
        printf("Retries for wifi connection exhausted. System halted.\n");
        return -1;
    }

    printf("Wifi connected.\n");
    printf("IP is %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));

    return 0;
}

void network_driver_poll()
{
    cyw43_arch_poll();
}

void network_driver_deinit()
{
    cyw43_arch_deinit();
}