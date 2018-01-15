
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
#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/f1/nvic.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include "sources/gpio.h"
#include "sources/rcc.h"
#include "sources/adc.h"
#include "sources/usart.h"
#include "sources/can.h"
#include "sources/timers.h"
#include "sources/defines.h"
#include "CANopen.h"
#include "stack/CO_driver.h"
#include <string.h>
uint8_t	upcount = 1;
uint32_t temp = 0x12345678;
uint8_t temp2;
uint16_t compare_time = 30000;
uint8_t channel_array[16];

int main(void)
{

	rcc_init();
/*while(reset != CO_RESET_APP){*/
/* CANopen communication reset - initialize CANopen objects *******************/
        CO_ReturnError_t err;
        /*uint16_t timer1msPrevious;*/

        /* disable CAN and CAN interrupts */


        /* initialize CANopen */
        err = CO_init(0/* CAN module address */, 120/* NodeID */, 125 /* bit rate */);
        if(err != CO_ERROR_NO){
		usart_send_string(USART1, "CANopen initialization failed, memory allocation error \n",
				strlen("CANopen initialization failed, memory allocation error \n"));
            while(1);
            /* CO_errorReport(CO->em, CO_EM_MEMORY_ALLOCATION_ERROR, CO_EMC_SOFTWARE_INTERNAL, err); */
        }


        /* Configure Timer interrupt function for execution every 1 millisecond */
	/*CO_TimerSetup();*/
	
        /* Configure CAN transmit and receive interrupt */


        /* start CAN */
        CO_CANsetNormalMode(CO->CANmodule[0]);

        /*reset = CO_RESET_NOT;*/
        /*timer1msPrevious = CO_timer1ms;*/
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
	/*CO_CANmodule_init();*/
	/*can_setup();*/
	usart_send_string(USART1, "BMS started \n", strlen("BMS started \n"));
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

void usb_hp_can_tx_isr(void){

	CO_CANinterrupt_Tx(CO->CANmodule[0]);
}

void usb_lp_can_rx0_isr(void){

	CO_CANinterrupt_Rx(CO->CANmodule[0]);
}
