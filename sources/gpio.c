
/************************************************************
 *	FILE NAME:
 *
 *	PURPUSE:
 *
 * 	FILE REFERENCES:
 * 	Name					I/O		Description
 *
 * 	EXTERNAL VARIABLES:
 * 	Source: < >
 * 	Name			Type		I/O		Description
 *
 * 	EXTERNAL REFERENCES:
 * 	Name							Description
 *	
 *	NOTES:
 *	
 *
 */
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include "gpio.h"

void gpio_init(){

	/* Enable GPIO clock. */
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	/* Enable led as output */
	gpio_set_mode(GREEN_LED_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, GREEN_LED);
	gpio_set_mode(RED_LED_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, RED_LED);
	gpio_set_mode(YELLOW_LED_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, YELLOW_LED);
	//Enable i2c1 SDA and SCL pins
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_I2C2_SCL | GPIO_I2C2_SDA);

	/*Enable pins for multiplexers*/

	gpio_set_mode(SELECT_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, A_PIN);
	gpio_set_mode(SELECT_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, C_PIN);
	gpio_set_mode(SELECT_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, B_PIN);

	gpio_set_mode(EN1_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, EN1_PIN);
	gpio_set_mode(EN2_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, EN2_PIN);
	/* Toggle LED to indicate compare event. */
	gpio_set(GREEN_LED_PORT, GREEN_LED);
}
uint8_t channel_set (uint8_t number){
	if (number == 0 || number > 12 )
	{
		return 0;
	}else{
		number -= 9;
		if (number < 8)
		{
			gpio_set(EN1_PORT, EN1_PIN);
			gpio_clear(SELECT_PORT, A_PIN | B_PIN | C_PIN);
			gpio_set(SELECT_PORT, (C_PIN & (number << 2)) | (B_PIN & (number << 3) | (A_PIN & (number << 4))));
			gpio_clear(EN2_PORT, EN2_PIN);

		}else{
			gpio_set(EN2_PORT, EN2_PIN);
			gpio_clear(SELECT_PORT, A_PIN | B_PIN | C_PIN);
			number -= 7;
			gpio_set(SELECT_PORT, (C_PIN & (number << 2)) | (B_PIN & (number << 3) | (A_PIN & (number << 4))));
			gpio_clear(EN1_PORT, EN1_PIN);

		}
		return 1;
	}
}
