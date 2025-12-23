/*
 * UDP GPIO inverter server for ZC702 (GEM0 + AXI GPIO loopback).
 */

#include <stdio.h>

#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"

#include "netif/xadapter.h"

#include "lwip/err.h"
#include "lwip/init.h"
#include "lwip/ip_addr.h"
#include "lwip/tcp.h"

#include "gpio_inverter.h"
#include "udp_inverter.h"

static struct netif server_netif;
struct netif *echo_netif;

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;
void tcp_fasttmr(void);
void tcp_slowtmr(void);

static void print_ip(const char *msg, ip_addr_t *ip)
{
	xil_printf("%s%d.%d.%d.%d\r\n", msg, ip4_addr1(ip), ip4_addr2(ip),
		   ip4_addr3(ip), ip4_addr4(ip));
}

static void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
	print_ip("Board IP  : ", ip);
	print_ip("Netmask   : ", mask);
	print_ip("Gateway   : ", gw);
}

static void print_app_header(void)
{
	xil_printf("\r\n--- lwIP UDP GPIO inverter ---\r\n");
	xil_printf("Board    : ZC702 (GEM0 via MIO/RGMII)\r\n");
	xil_printf("Service  : UDP port %d\r\n", UDP_INVERTER_PORT);
	xil_printf("Payload  : 4-byte little-endian word -> reply [W][R] (8 bytes)\r\n\r\n");
}

int main(void)
{
	ip_addr_t ipaddr, netmask, gw;
	unsigned char mac_ethernet_address[] = {0x00, 0x0a, 0x35, 0x00, 0x01, 0x02};

	echo_netif = &server_netif;

	init_platform();

	IP4_ADDR(&ipaddr, 192, 168, 1, 10);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 1, 1);

	print_app_header();

	lwip_init();

	if (!xemac_add(echo_netif, &ipaddr, &netmask, &gw, mac_ethernet_address,
		       XPAR_XEMACPS_0_BASEADDR)) {
		xil_printf("Error adding N/W interface\r\n");
		return -1;
	}

	netif_set_default(echo_netif);

#ifndef SDT
	platform_enable_interrupts();
#endif

	netif_set_up(echo_netif);
	print_ip_settings(&ipaddr, &netmask, &gw);

	if (gpio_inverter_init() != 0) {
		xil_printf("AXI GPIO init failed\r\n");
		return -1;
	}

	if (udp_inverter_init() != ERR_OK) {
		xil_printf("UDP inverter init failed\r\n");
		return -1;
	}

	while (1) {
		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
		xemacif_input(echo_netif);
	}

	/* Never reached */
	cleanup_platform();
	return 0;
}
