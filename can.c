#include <libopencm3/stm32/can.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include "includes/can.h"
#include "includes/defines.h"
#include "includes/usart.h"
void can_setup(void)
{
	/* Enable peripheral clocks. */
	rcc_periph_clock_enable(RCC_AFIO);
	/*rcc_periph_clock_enable(RCC_GPIOB);*/
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_CAN1);

	/*AFIO_MAPR |= AFIO_MAPR_CAN1_REMAP_PORTB;*/

	/*[> Configure CAN pin: RX (input pull-up). <]*/
	/*gpio_set_mode(GPIO_BANK_CAN1_PB_RX, GPIO_MODE_INPUT,*/
		      /*GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_CAN1_PB_RX);*/
	/*gpio_set(GPIO_BANK_CAN1_PB_RX, GPIO_CAN1_PB_RX);*/

	/*[> Configure CAN pin: TX. <]*/
	/*gpio_set_mode(GPIO_BANK_CAN1_PB_TX, GPIO_MODE_OUTPUT_50_MHZ,*/
		      /*GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_CAN1_PB_TX);*/

	/* Configure CAN pin: RX (input pull-up). */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
		      GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_CAN_RX);
	gpio_set(GPIOA, GPIO_CAN_RX);

	/* Configure CAN pin: TX. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_CAN_TX);
	/* NVIC setup. */
	nvic_enable_irq(NVIC_USB_LP_CAN_RX0_IRQ);
	nvic_set_priority(NVIC_USB_LP_CAN_RX0_IRQ, 1);

	/* Reset CAN. */
	can_reset(CAN1);

	/* CAN cell init. */
	can_init(CAN1,
		     false,           /* TTCM: Time triggered comm mode? */
		     true,            /* ABOM: Automatic bus-off management? */
		     false,           /* AWUM: Automatic wakeup mode? */
		     false,           /* NART: No automatic retransmission? */
		     false,           /* RFLM: Receive FIFO locked mode? */
		     false,           /* TXFP: Transmit FIFO priority? */
		     CAN_BTR_SJW_1TQ,
		     CAN_BTR_TS1_3TQ,
		     CAN_BTR_TS2_4TQ,
		     12,		/* BRP+1: Baud rate prescaler */
		     false,		/*Loopback*/	
		     false);		/*Silent*/             	
	
	/* CAN filter 0 init. */
	can_filter_id_mask_32bit_init(CAN1,
				0,     /* Filter ID */
				0,     /* CAN ID */
				0,     /* CAN ID mask */
				0,     /* FIFO assignment (here: FIFO0) */
				true); /* Enable the filter. */

	/* Enable CAN RX interrupt. */
	can_enable_irq(CAN1, CAN_IER_FMPIE0);
}
void usb_lp_can_rx0_isr(void)
{
	uint32_t id;
	bool ext, rtr;
	uint8_t fmi, length, data[8];

	can_receive(CAN1, 0, false, &id, &ext, &rtr, &fmi, &length, data, 0);
	gpio_toggle(GREEN_LED_PORT, GREEN_LED);
	usart_send_string(USART1, "\n INT", 5);	

/*
 *        if (data[0] & 1)
 *                gpio_clear(GPIOA, GPIO8);
 *        else
 *                gpio_set(GPIOA, GPIO8);
 *
 *        if (data[0] & 2)
 *                gpio_clear(GPIOB, GPIO4);
 *        else
 *                gpio_set(GPIOB, GPIO4);
 *
 *        if (data[0] & 4)
 *                gpio_clear(GPIOC, GPIO2);
 *        else
 *                gpio_set(GPIOC, GPIO2);
 *
 *        if (data[0] & 8)
 *                gpio_clear(GPIOC, GPIO5);
 *        else
 *                gpio_set(GPIOC, GPIO5);
 *
 */
	can_fifo_release(CAN1, 0);
	uint16_t i = 6000;
	while(i != 0)
	{
		i--;
	usart_send_string(USART1, i, 1);	
	}
	can_send_test();
}
void can_send_test(void)
{
	static int temp32 = 0;
	static uint8_t data[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	/*usart_send_32(USART1, CAN_TSR(CAN1), 4);*/
	/* We call this handler every 1ms so 100ms = 1s
	 * Resulting in 100Hz message frequency.
	 */
	/*if (++temp32 != 100)*/
		/*return;*/

	/*temp32 = 0;*/

	/*[> Transmit CAN frame. <]*/
	/*data[0]++;*/
	/*usart_send_32(USART1, &CAN_RF0R(CAN1), 4);*/
	if (can_transmit(CAN1,
			 0,     /* (EX/ST)ID: CAN ID */
			 false, /* IDE: CAN ID extended? */
			 false, /* RTR: Request transmit? */
			 8,     /* DLC: Data length */
			 data) == -1)
	{
	/*gpio_toggle(GREEN_LED_PORT, GREEN_LED);*/
	}
	usart_send_string(USART1, "\n CAN", 5);	
	/*usart_send_string(USART1, "\n Status reg: ", strlen("\n Status reg: "));*/
}
