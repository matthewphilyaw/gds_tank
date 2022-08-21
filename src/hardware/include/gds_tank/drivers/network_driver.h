#ifndef GDS_TANK_NETWORK_DRIVER_H
#define GDS_TANK_NETWORK_DRIVER_H

#include "pico/stdlib.h"

uint network_driver_init();
void network_driver_poll();
void network_driver_deinit();

#endif //GDS_TANK_NETWORK_DRIVER_H
