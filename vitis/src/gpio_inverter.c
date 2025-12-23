/*
 * Drive the AXI GPIO inverter chain: AXI GPIO OUT -> NOT -> AXI GPIO IN.
 */

#include "gpio_inverter.h"

#include "xgpio_l.h"
#include "xil_io.h"
#include "xparameters.h"

int gpio_inverter_init(void)
{
	/* Channel 1 direction: OUT drives, IN is input. */
	Xil_Out32(XPAR_AXI_GPIO_OUT_BASEADDR + XGPIO_TRI_OFFSET, 0x00000000U);
	Xil_Out32(XPAR_AXI_GPIO_IN_BASEADDR + XGPIO_TRI_OFFSET, 0xFFFFFFFFU);

	return 0;
}

int gpio_inverter_write_read(u32 value, u32 *readback)
{
	if (readback == NULL) {
		return -1;
	}

	Xil_Out32(XPAR_AXI_GPIO_OUT_BASEADDR + XGPIO_DATA_OFFSET, value);
	*readback = Xil_In32(XPAR_AXI_GPIO_IN_BASEADDR + XGPIO_DATA_OFFSET);

	return 0;
}
