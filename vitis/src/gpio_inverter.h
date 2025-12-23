/*
 * Simple AXI GPIO accessors for the inverter loopback.
 */

#ifndef GPIO_INVERTER_H
#define GPIO_INVERTER_H

#include "xil_types.h"

int gpio_inverter_init(void);
int gpio_inverter_write_read(u32 value, u32 *readback);

#endif
