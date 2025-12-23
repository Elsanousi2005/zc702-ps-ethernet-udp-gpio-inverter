/*
 * Platform hooks for lwIP bare-metal apps (Zynq/SDT template).
 */

#ifndef __PLATFORM_H_
#define __PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "xil_types.h"

/* Platform timer is calibrated for 50 ms, so keep interval value 20 to call
 * eth_link_detect() at every one second.
 */
#define ETH_LINK_DETECT_INTERVAL 20

void init_platform(void);
void cleanup_platform(void);
#ifdef SDT
void init_timer(void);
void TimerCounterHandler(void *CallBackRef __attribute__((unused)), u32 TmrCtrNumber __attribute__((unused)));
#endif
#ifdef __MICROBLAZE__
void timer_callback(void);
#endif
#ifdef __PPC__
void timer_callback(void);
#endif
void platform_setup_timer(void);
void platform_enable_interrupts(void);

#ifdef __cplusplus
}
#endif

#endif
