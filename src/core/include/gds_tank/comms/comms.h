//
// Created by Tommy Philyaw on 8/20/22.
//

#ifndef GDS_TANK_COMMS_H
#define GDS_TANK_COMMS_H

#include "stdint.h"

uint32_t comms_init();
void comms_poll();
void comms_deinit();

#endif //GDS_TANK_COMMS_H
