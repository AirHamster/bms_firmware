#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include "includes/gpio.h"
#include "includes/rcc.h"
#include "includes/adc.h"
#include "includes/usart.h"
#include "includes/can.h"
#include "includes/timers.h"
#include "includes/defines.h"
uint8_t	upcount = 1;
uint32_t temp = 0x12345678;
uint8_t temp2;
uint16_t compare_time = 30000;


int main(void)
{

	rcc_init();
	gpio_init();
	usart_init();
	adc_init();
	/*tim1_init();*/
	can_setup();
	/*usart_send_byte(USART1, 'h');*/
	usart_send_string(USART1, "Hello \n", strlen("Hello \n"));
	gpio_clear(GREEN_LED_PORT, GREEN_LED);
	int i;
        while (1) {
                //gpio_toggle(LEDPORT, LED);	/* LED on/off */
                for (i = 0; i < 800000; i++)	/* Wait a bit. */
			__asm__("nop");
	}

	return 0;
}

