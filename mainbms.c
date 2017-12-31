
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
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include "includes/gpio.h"
#include "includes/rcc.h"
#include "includes/adc.h"
#include "includes/usart.h"
#include "includes/can.h"
#include "includes/timers.h"
#include "includes/defines.h"
#include <string.h>
uint8_t	upcount = 1;
uint32_t temp = 0x12345678;
uint8_t temp2;
uint16_t compare_time = 30000;
uint8_t channel_array[16];

int main(void)
{

	rcc_init();
	gpio_init();
	usart_init();
	adc_init();
	/* Select the channel we want to convert. 16=temperature_sensor. */
	channel_array[0] = 16;
	/* Set the injected sequence here, with number of channels */
	adc_set_regular_sequence(ADC1, 1, channel_array);

	channel_array[0] = 8;
	/* Set the injected sequence here, with number of channels */
	adc_set_regular_sequence(ADC2, 1, channel_array);

	tim1_init();
	can_setup();
	/*usart_send_byte(USART1, 'h');*/
	usart_send_string(USART1, "Hello \n", strlen("Hello \n"));
	gpio_clear(GREEN_LED_PORT, GREEN_LED);
	/*gpio_set(YELLOW_LED_PORT, YELLOW_LED);*/
	int i;
        while (1) {
                //gpio_toggle(LEDPORT, LED);	/* LED on/off */
                for (i = 0; i < 800000; i++)	/* Wait a bit. */
			__asm__("nop");
	}

	return 0;
}

