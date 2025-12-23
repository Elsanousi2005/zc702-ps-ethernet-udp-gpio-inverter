/*
 * UDP inverter server declarations.
 */

#ifndef UDP_INVERTER_H
#define UDP_INVERTER_H

#include "lwip/err.h"

#define UDP_INVERTER_PORT 5005

err_t udp_inverter_init(void);

#endif
