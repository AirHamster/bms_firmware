#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "includes/gpio.h"
#include "includes/rcc.h"
#include "includes/adc.h"
#include "includes/usart.h"
#include "includes/can.h"
#define LEDPORT GPIOC

#define LED GPIO13


int main(void)
{

	rcc_init();
	gpio_init();
	usart_init();
	adc_init();
	can_setup();
	/* Blink the LED (PC12) on the board. */
        gpio_set(LEDPORT, LED);
	int i;
        while (1) {
                //gpio_toggle(LEDPORT, LED);	/* LED on/off */
                for (i = 0; i < 800000; i++)	/* Wait a bit. */
			__asm__("nop");
	}

	return 0;
}

