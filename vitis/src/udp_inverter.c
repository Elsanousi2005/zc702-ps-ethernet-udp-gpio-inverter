/*
 * UDP service: write a 32-bit word to AXI GPIO OUT and return [W][R].
 */

#include "udp_inverter.h"

#include <string.h>

#include "gpio_inverter.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "xil_printf.h"

static struct udp_pcb *inverter_pcb;

static void udp_inverter_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,
			      const ip_addr_t *addr, u16_t port)
{
	LWIP_UNUSED_ARG(arg);
	if (p == NULL) {
		return;
	}

	if (p->tot_len == sizeof(u32)) {
		u32 write_word = 0U;
		u32 read_word = 0U;

		if (pbuf_copy_partial(p, &write_word, sizeof(write_word), 0) ==
		    sizeof(write_word)) {
			if (gpio_inverter_write_read(write_word, &read_word) == 0) {
				struct pbuf *reply =
					pbuf_alloc(PBUF_TRANSPORT,
						   sizeof(write_word) + sizeof(read_word),
						   PBUF_RAM);
				if (reply != NULL) {
					memcpy(reply->payload, &write_word, sizeof(write_word));
					memcpy((u8_t *)reply->payload + sizeof(write_word),
					       &read_word, sizeof(read_word));
					udp_sendto(pcb, reply, addr, port);
					pbuf_free(reply);
				} else {
					xil_printf("udp_inverter: reply alloc failed\r\n");
				}
			}
		}
	} else {
		static const char err_msg[] = "BADLEN";
		struct pbuf *reply = pbuf_alloc(PBUF_TRANSPORT,
						sizeof(err_msg) - 1U, PBUF_RAM);
		if (reply != NULL) {
			memcpy(reply->payload, err_msg, sizeof(err_msg) - 1U);
			udp_sendto(pcb, reply, addr, port);
			pbuf_free(reply);
		}
	}

	pbuf_free(p);
}

err_t udp_inverter_init(void)
{
	inverter_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
	if (inverter_pcb == NULL) {
		xil_printf("udp_inverter: failed to create PCB\r\n");
		return ERR_MEM;
	}

	err_t err = udp_bind(inverter_pcb, IP_ANY_TYPE, UDP_INVERTER_PORT);
	if (err != ERR_OK) {
		xil_printf("udp_inverter: bind failed (%d)\r\n", err);
		udp_remove(inverter_pcb);
		inverter_pcb = NULL;
		return err;
	}

	udp_recv(inverter_pcb, udp_inverter_recv, NULL);
	xil_printf("udp_inverter: listening on UDP port %d\r\n", UDP_INVERTER_PORT);

	return ERR_OK;
}
