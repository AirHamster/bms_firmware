#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include "includes/gpio.h"
#include "includes/defines.h"
void gpio_init(){

	/* Enable GPIOA clock (for LED GPIOs). */
	rcc_periph_clock_enable(RCC_GPIOC);
	/* Enable led as output */
	gpio_set_mode(GREEN_LED_PORT, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, GREEN_LED);
	//Enable i2c1 SDA and SCL pins
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_I2C2_SCL | GPIO_I2C2_SDA);

	/* Toggle LED to indicate compare event. */
	gpio_set(GREEN_LED_PORT, GREEN_LED);
}
