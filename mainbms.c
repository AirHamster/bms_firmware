#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/f1/nvic.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/can.h>
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
CO_CANtx_t *txbuff;
CO_CANrx_t *rxbuff;
void (*fpointer)(void *, const CO_CANrxMsg_t *);
void resieveProc(void *object, const CO_CANrxMsg_t *message);

void can_send_test(uint8_t number, uint16_t msg);
int main(void)
{
	fpointer = resieveProc;
	rcc_init();
	gpio_init();
	usart_init();

	adc_init();

	tim1_init();
	/*can_setup();*/
	usart_send_string(USART1, "BMS started \n", strlen("BMS started \n"));

	/* CANopen communication reset - initialize CANopen objects *******************/
        CO_ReturnError_t err;

        /* disable CAN and CAN interrupts */


        /* initialize CANopen */
        err = CO_init(0/* CAN module address */, 120/* NodeID */, 125 /* bit rate */);
        if(err != CO_ERROR_NO){
		usart_send_string(USART1, "CANopen initialization failed, memory allocation error \n",
				strlen("CANopen initialization failed, memory allocation error \n"));
            while(1);
            /* CO_errorReport(CO->em, CO_EM_MEMORY_ALLOCATION_ERROR, CO_EMC_SOFTWARE_INTERNAL, err); */
        }else{
		usart_send_string(USART1, "CANopen initialized sucessfully \n",
				strlen("CANopen initialized sucessfully \n"));
	}


        /* Configure Timer interrupt function for execution every 1 millisecond */
	CO_TimerSetup();

        /* start CAN */
        CO_CANsetNormalMode(CO->CANmodule[0]);
	txbuff = CO_CANtxBufferInit(CO->CANmodule[0], 0x1010, 126, false, 8, 0);
	CO_CANrxBufferInit(CO->CANmodule[0], 0x1011, 126, 0x7FF, false, 0x1400, fpointer);

	/*gpio_clear(GREEN_LED_PORT, GREEN_LED); //this for BluePill, low is active*/
	/*gpio_set(YELLOW_LED_PORT, YELLOW_LED); //this for bms, high is active*/

	int i;
        while (1) {
                for (i = 0; i < 800000; i++)	/* Wait a bit. */
			__asm__("nop");
	}

	return 0;
}
void sys_tick_handler(void){

    if(CO->CANmodule[0]->CANnormal) {
        bool_t syncWas;

        /* Process Sync and read inputs */
        syncWas = CO_process_SYNC_RPDO(CO, 1000);

        /* Re-enable CANrx, if it was disabled by SYNC callback */
        /*CO_CAN_ISR_ENABLE = 1;*/

        /* Further I/O or nonblocking application code may go here. */

        /* Write outputs */
        CO_process_TPDO(CO, syncWas, 1000);

    }
}
void usb_hp_can_tx_isr(void){

	CO_CANinterrupt_Tx(CO->CANmodule[0]);
}

void usb_lp_can_rx0_isr(void){

	gpio_clear(GREEN_LED_PORT, GREEN_LED);
	usart_send_string(USART1, "Resieved!\n", 10);
	CO_CANinterrupt_Rx(CO->CANmodule[0]);
}
void resieveProc(void *object, const CO_CANrxMsg_t *message){
}

void can_send_test(uint8_t number, uint16_t msg)
{
	/*static int temp32 = 0;*/
	uint8_t data[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	data[0] = number;
	data[1] = msg >> 8;
	data[2] = msg & 0xFF;
	usart_send_string(USART1, "OLD send\n", 10);	
	if (can_transmit(CAN1,
			 0,     /* (EX/ST)ID: CAN ID */
			 false, /* IDE: CAN ID extended? */
			 false, /* RTR: Request transmit? */
			 8,     /* DLC: Data length */
			 data) == -1)
	{
	}
}
