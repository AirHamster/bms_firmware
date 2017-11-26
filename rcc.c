#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/can.h>
#include <libopencm3/stm32/gpio.h>
#include "includes/rcc.h"

void rcc_init(void){
	rcc_clock_setup_in_hsi_out_64mhz();

	/*Enable clocks for I2C*/
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_USART1);

}

void sys_tick_handler(void)
{
	static int temp32 = 0;
	static uint8_t data[8] = {0, 1, 2, 0, 0, 0, 0, 0};

	/* We call this handler every 1ms so every 100ms = 0.1s
	 * resulting in 100Hz message rate.
	 */
	if (++temp32 != 100)
		return;

	temp32 = 0;

	/* Transmit CAN frame. */
	data[0]++;
	if (can_transmit(CAN1,
			 0,     /* (EX/ST)ID: CAN ID */
			 false, /* IDE: CAN ID extended? */
			 false, /* RTR: Request transmit? */
			 8,     /* DLC: Data length */
			 data) == -1)
	{
		gpio_set(GPIOA, GPIO8);    /* LED1 off */
		gpio_set(GPIOB, GPIO4);    /* LED2 off */
		gpio_set(GPIOC, GPIO2);    /* LED3 off */
		gpio_set(GPIOC, GPIO5);    /* LED4 off */
		gpio_clear(GPIOC, GPIO15); /* LED5 on */
	}
}
