/*
 * CAN module object for generic microcontroller.
 *
 * This file is a template for other microcontrollers.
 *
 * @file        CO_driver.c
 * @ingroup     CO_driver
 * @author      Janez Paternoster
 * @copyright   2004 - 2015 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free and open source software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Following clarification and special exception to the GNU General Public
 * License is included to the distribution terms of CANopenNode:
 *
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library. Thus, the terms and
 * conditions of the GNU General Public License cover the whole combination.
 *
 * As a special exception, the copyright holders of this library give
 * you permission to link this library with independent modules to
 * produce an executable, regardless of the license terms of these
 * independent modules, and to copy and distribute the resulting
 * executable under terms of your choice, provided that you also meet,
 * for each linked independent module, the terms and conditions of the
 * license of that module. An independent module is a module which is
 * not derived from or based on this library. If you modify this
 * library, you may extend this exception to your version of the
 * library, but you are not obliged to do so. If you do not wish
 * to do so, delete this exception statement from your version.
 */


#include "CO_driver.h"
#include "CO_Emergency.h"

#include <libopencm3/stm32/can.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

static void CO_CANClkSetting (void);
static void CO_CANconfigGPIO (void);
static void CO_CANsendToModule(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer, uint8_t transmit_mailbox);

/******************************************************************************/
void CO_CANsetConfigurationMode(int32_t CANbaseAddress){
    /* Put CAN module in configuration mode */
}


/******************************************************************************/
void CO_CANsetNormalMode(CO_CANmodule_t *CANmodule){
    /* Put CAN module in normal mode */

    CANmodule->CANnormal = true;
}


/******************************************************************************/
CO_ReturnError_t CO_CANmodule_init(
        CO_CANmodule_t         *CANmodule,
        uint32_t                 *CANbaseAddress,
        CO_CANrx_t              rxArray[],
        uint16_t                rxSize,
        CO_CANtx_t              txArray[],
        uint16_t                txSize,
        uint16_t                CANbitRate)
{
	int i;
	uint8_t result;
	uint8_t prescaler;

	/* verify arguments */
	if(CANmodule==NULL || rxArray==NULL || txArray==NULL){
		return CO_ERROR_ILLEGAL_ARGUMENT;
	}

	CANmodule->CANbaseAddress = CANbaseAddress;
	CANmodule->rxArray = rxArray;
	CANmodule->rxSize = rxSize;
	CANmodule->txArray = txArray;
	CANmodule->txSize = txSize;
	CANmodule->CANnormal = false;
	CANmodule->useCANrxFilters = false;
	CANmodule->bufferInhibitFlag = 0;
	CANmodule->firstCANtxMessage = 1;
	CANmodule->CANtxCount = 0;
	CANmodule->errOld = 0;
	CANmodule->em = 0;

	nvic_disable_irq(NVIC_USB_HP_CAN_TX_IRQ);
	nvic_disable_irq(NVIC_USB_LP_CAN_RX0_IRQ);

	for (i = 0; i < rxSize; i++)
	{
		CANmodule->rxArray[i].ident = 0;
		CANmodule->rxArray[i].pFunct = 0;
	}
	for (i = 0; i < txSize; i++)
	{
		CANmodule->txArray[i].bufferFull = 0;
	}

	/* Setting Clock of CAN HW */
	CO_CANClkSetting();

	/* GPIO Config for CAN */
	CO_CANconfigGPIO();

	/* Init CAN controler */
	can_reset(CAN1);

	switch (CANbitRate)
	{
		case 1000: prescaler = 2;
			   break;
		case 500: prescaler = 4;
			  break;
		default:
		case 250: prescaler = 8;
			  break;
		case 125: prescaler = 16;
			  break;
		case 100: prescaler = 20;
			  break;
		case 50: prescaler = 40;
			 break;
		case 20: prescaler = 100;
			 break;
		case 10: prescaler = 200;
			 break;
	}
	 /* TODO: calc bitrate/values*/
	/*
	 *CAN_InitStruct.CAN_SJW = CAN_SJW_4tq;     // changed by VJ, old value = CAN_SJW_1tq;
	 *CAN_InitStruct.CAN_BS1 = CAN_BS1_12tq;    // changed by VJ, old value = CAN_BS1_3tq;
	 *CAN_InitStruct.CAN_BS2 = CAN_BS2_5tq;     // changed by VJ, old value = CAN_BS2_2tq;
	 *CAN_InitStruct.CAN_NART = ENABLE;   // No Automatic retransmision
	 */

	/*TODO: dependable can1 / can2*/
	usart_send_string(USART1, "Inside init\n", 12);
	result = can_init(CAN1,
			false,           /* TTCM: Time triggered comm mode? */
			true,            /* ABOM: Automatic bus-off management? */
			false,           /* AWUM: Automatic wakeup mode? */
			false,           /* NART: No automatic retransmission? */
			false,           /* RFLM: Receive FIFO locked mode? */
			false,           /* TXFP: Transmit FIFO priority? */
			CAN_BTR_SJW_1TQ,
			CAN_BTR_TS1_3TQ,
			CAN_BTR_TS2_4TQ,
			prescaler,		/* BRP+1: Baud rate prescaler */
			false,		/*Loopback*/	
			false);		/*Silent*/             	
	if (result != 0)
	{
		return CO_ERROR_TIMEOUT;  /* CO- Return Init failed */
	}

	/* CAN filter 0 init. */
	can_filter_id_mask_32bit_init(CAN1,
			0,     /* Filter ID */
			0,     /* CAN ID */
			0,     /* CAN ID mask */
			0,     /* FIFO assignment (here: FIFO0) */
			true); /* Enable the filter. */

	/* NVIC setup. */
	nvic_enable_irq(NVIC_USB_HP_CAN_TX_IRQ);
	nvic_enable_irq(NVIC_USB_LP_CAN_RX0_IRQ);

	nvic_set_priority(NVIC_USB_HP_CAN_TX_IRQ, 1);
	nvic_set_priority(NVIC_USB_LP_CAN_RX0_IRQ, 1);

	/* Enable CAN RX interrupt. */
	can_enable_irq(CAN1, CAN_IER_FMPIE0);
	can_enable_irq(CAN1, CAN_IER_TMEIE);

	return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANmodule_disable(CO_CANmodule_t *CANmodule){
	/* Reset CAN. */
	can_disable_irq(CAN1, CAN_IER_FMPIE0);
	can_disable_irq(CAN1, CAN_IER_TMEIE);

	nvic_disable_irq(NVIC_USB_HP_CAN_TX_IRQ);
	nvic_disable_irq(NVIC_USB_LP_CAN_RX0_IRQ);

	can_reset(CAN1);
}


/******************************************************************************/
uint16_t CO_CANrxMsg_readIdent(const CO_CANrxMsg_t *rxMsg){
    return (uint16_t) rxMsg->ident;
}


/******************************************************************************/
CO_ReturnError_t CO_CANrxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint16_t                mask,
        bool_t                  rtr,
        void                   *object,
        void                  (*pFunct)(void *object, const CO_CANrxMsg_t *message))
{
   CO_CANrx_t *rxBuffer;
    uint16_t RXF, RXM;

    //safety
    if (!CANmodule || !object || !pFunct || index >= CANmodule->rxSize)
    {
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    //buffer, which will be configured
    rxBuffer = CANmodule->rxArray + index;

    //Configure object variables
    rxBuffer->object = object;
    rxBuffer->pFunct = pFunct;


    //CAN identifier and CAN mask, bit aligned with CAN module registers
    RXF = (ident & 0x07FF) << 2;
    if (rtr) RXF |= 0x02;
    RXM = (mask & 0x07FF) << 2;
    RXM |= 0x02;

    //configure filter and mask
    if (RXF != rxBuffer->ident || RXM != rxBuffer->mask)
    {
        rxBuffer->ident = RXF;
        rxBuffer->mask = RXM;
    }

    return CO_ERROR_NO;
}


/******************************************************************************/
CO_CANtx_t *CO_CANtxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        bool_t                  rtr,
        uint8_t                 noOfBytes,
        bool_t                  syncFlag)
{
    uint32_t TXF;
    CO_CANtx_t *buffer;

    //safety
    if (!CANmodule || CANmodule->txSize <= index) return 0;

    //get specific buffer
    buffer = &CANmodule->txArray[index];

    //CAN identifier, bit aligned with CAN module registers

    TXF = ident << 21;
    TXF &= 0xFFE00000;
    if (rtr) TXF |= 0x02;

    //write to buffer
    buffer->ident = TXF;
    buffer->DLC = noOfBytes;
    buffer->bufferFull = 0;
    buffer->syncFlag = syncFlag ? 1 : 0;

    return buffer;
}


/******************************************************************************/
CO_ReturnError_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer){
    CO_ReturnError_t err = CO_ERROR_NO;
    int8_t txBuff;

    /* Verify overflow */ 
    if(buffer->bufferFull)
    {
        if(!CANmodule->firstCANtxMessage)/* don't set error, if bootup message is still on buffers */
            CO_errorReport((CO_EM_t*)CANmodule->em, CO_EM_CAN_TX_OVERFLOW, CO_EMC_CAN_OVERRUN, 0);
        err = CO_ERROR_TX_OVERFLOW;
    }

    CO_LOCK_CAN_SEND();
    //if CAN TB buffer0 is free, copy message to it
     txBuff = getFreeTxBuff(CANmodule);
   // #error change this - use only one buffer for transmission - see generic driver
    if(txBuff != 0 && CANmodule->CANtxCount == 0)
    {
        CANmodule->bufferInhibitFlag = buffer->syncFlag;
        CO_CANsendToModule(CANmodule, buffer, txBuff);
    }
    //if no buffer is free, message will be sent by interrupt
    else
    {
        buffer->bufferFull = 1;
        CANmodule->CANtxCount++;
        // vsechny buffery jsou plny, musime povolit preruseni od vysilace, odvysilat az v preruseni
        //CAN_ITConfig(CANmodule->CANbaseAddress, CAN_IT_TME, ENABLE);
    }
    CO_UNLOCK_CAN_SEND();

    return err;
}


/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule){
   //TODO: razobratsya s etim
    uint32_t tpdoDeleted = 0U;

    CO_LOCK_CAN_SEND();
    /* Abort message from CAN module, if there is synchronous TPDO.
     * Take special care with this functionality. */
    if(/*messageIsOnCanBuffer && */CANmodule->bufferInhibitFlag){
        /* clear TXREQ */
        CANmodule->bufferInhibitFlag = false;
        tpdoDeleted = 1U;
    }
    /* delete also pending synchronous TPDOs in TX buffers */
    if(CANmodule->CANtxCount != 0U){
        uint16_t i;
        CO_CANtx_t *buffer = &CANmodule->txArray[0];
        for(i = CANmodule->txSize; i > 0U; i--){
            if(buffer->bufferFull){
                if(buffer->syncFlag){
                    buffer->bufferFull = false;
                    CANmodule->CANtxCount--;
                    tpdoDeleted = 2U;
                }
            }
            buffer++;
        }
    }
    CO_UNLOCK_CAN_SEND();

    if(tpdoDeleted != 0U){
        CO_errorReport((CO_EM_t*)CANmodule->em, CO_EM_TPDO_OUTSIDE_WINDOW, CO_EMC_COMMUNICATION, tpdoDeleted);
    }
}


/******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule){
   uint32_t err;
   CO_EM_t* em = (CO_EM_t*)CANmodule->em;
//TODO:
   //err = CANmodule->CANbaseAddress->ESR;
   // if(CAN_REG(CANmodule->CANbaseAddress, C_INTF) & 4) err |= 0x80;
/*
   if(CANmodule->errOld != err)
   {
      CANmodule->errOld = err;

      //CAN RX bus overflow
     // if(CANmodule->CANbaseAddress->RF0R & 0x08)
      {
         CO_errorReport(em, CO_EM_CAN_RXB_OVERFLOW, CO_EMC_CAN_OVERRUN, err);
      //   CANmodule->CANbaseAddress->RF0R &=~0x08;//clear bits
      }

      //CAN TX bus off
      if(err & 0x04) CO_errorReport(em, CO_EM_CAN_TX_BUS_OFF, CO_EMC_BUS_OFF_RECOVERED, err);
      else           CO_errorReset(em, CO_EM_CAN_TX_BUS_OFF, err);

      //CAN TX or RX bus passive
      if(err & 0x02)
      {
         if(!CANmodule->firstCANtxMessage) CO_errorReport(em, CO_EM_CAN_TX_BUS_PASSIVE, CO_EMC_CAN_PASSIVE, err);
      }
      else
      {
        // int16_t wasCleared;
       // /* wasCleared = CO_errorReset(em, CO_EM_CAN_TX_BUS_PASSIVE, err);
        /* if(wasCleared == 1) CO_errorReset(em, CO_EM_CAN_TX_OVERFLOW, err);
      }


      //CAN TX or RX bus warning
      if(err & 0x01)
      {
         CO_errorReport(em, CO_EM_CAN_BUS_WARNING, CO_EMC_NO_ERROR, err);
      }
      else
      {
         CO_errorReset(em, CO_EM_CAN_BUS_WARNING, err);
      }
   } */
}


/******************************************************************************/
// Interrupt from Receiver
void CO_CANinterrupt_Rx(CO_CANmodule_t *CANmodule)
{

	uint32_t id;
	bool ext, rtr;
	uint8_t fmi, length, data[8];

	can_receive(CAN1, 0, false, &id, &ext, &rtr, &fmi, &length, data, 0);


	/*CanRxMsg      CAN1_RxMsg;*/

	/*CAN_Receive(CANmodule->CANbaseAddress, CAN_FilterFIFO0, &CAN1_RxMsg);*/
	can_receive(CAN1, 0, false, &id, &ext, &rtr, &fmi, &length, data, 0);
	{
		uint16_t index;
		uint8_t msgMatched = 0;
		CO_CANrx_t *msgBuff = CANmodule->rxArray;
		for (index = 0; index < CANmodule->rxSize; index++)
		{
			//TODO: RTR processing
			uint16_t msg = (id << 2);
			if (((msg ^ msgBuff->ident) & msgBuff->mask) == 0)
			{
				msgMatched = 1;
				break;
			}
			msgBuff++;
		}
		//Call specific function, which will process the message
		if (msgMatched && msgBuff->pFunct)
			msgBuff->pFunct(msgBuff->object, data);
	}
	can_fifo_release(CAN1, 0);
}

/******************************************************************************/
// Interrupt from Transeiver
void CO_CANinterrupt_Tx(CO_CANmodule_t *CANmodule)
{

     int8_t txBuff;
    /* First CAN message (bootup) was sent successfully */
    CANmodule->firstCANtxMessage = 0;
    /* clear flag from previous message */
    CANmodule->bufferInhibitFlag = 0;
    /* Are there any new messages waiting to be send */
    if(CANmodule->CANtxCount > 0)
    {
        uint16_t i;             /* index of transmitting message */

        /* first buffer */
        CO_CANtx_t *buffer = CANmodule->txArray;
        /* search through whole array of pointers to transmit message buffers. */
        for(i = CANmodule->txSize; i > 0; i--)
        {
            /* if message buffer is full, send it. */
            if(buffer->bufferFull)
            {
                buffer->bufferFull = 0;
                CANmodule->CANtxCount--;
    txBuff = getFreeTxBuff(CANmodule);    //VJ
                /* Copy message to CAN buffer */
                CANmodule->bufferInhibitFlag = buffer->syncFlag;
                CO_CANsendToModule(CANmodule, buffer, txBuff);
                break;                      /* exit for loop */
            }
            buffer++;
        }/* end of for loop */

        /* Clear counter if no more messages */
        if(i == 0) CANmodule->CANtxCount = 0;
    }
}

/******************************************************************************/
void CO_CANinterrupt_Status(CO_CANmodule_t *CANmodule)
{
  // status is evalved with pooling
}

/******************************************************************************/
static void CO_CANsendToModule(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer, uint8_t transmit_mailbox)
{
	(void) transmit_mailbox;
	(void*) CANmodule;
	int i;
	uint8_t canID;
	uint8_t dat_lenth;
	uint8_t data[8];

	for (i = 0; i < 8; i++) 
		data[i] = buffer->data[i];

	canID = ((buffer->ident) >> 21);
	dat_lenth = buffer->DLC;
	usart_send_string(USART1, "Inside sendToModule\n", sizeof("Inside sendToModule\n")-1);
	if (can_transmit(CAN1,
				canID,     /* (EX/ST)ID: CAN ID */
				false, /* IDE: CAN ID extended? */
				false, /* RTR: Request transmit? */
				dat_lenth,     /* DLC: Data length */
				data) == -1)
	{
		/*gpio_toggle(GREEN_LED_PORT, GREEN_LED);*/
		usart_send_string(USART1, "TR failed, no free mailboxes\n",
				sizeof("TR failed, no free mailboxes\n"));
	}
}

int8_t getFreeTxBuff(CO_CANmodule_t *CANmodule)
{
	return (int8_t)can_available_mailbox(CAN1);
}

/* CO- VJ Changed Start */
/******************************************************************************/
static void CO_CANClkSetting (void)
{

	/*TODO: make configuratoin depends on CANmodule 1/2*/
	/* Enable peripheral clocks. */
	/*rcc_periph_clock_enable(RCC_AFIO);*/
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_CAN1);
}

void CO_TimerSetup (void)
{
	/* 72MHz / 8 => 9000000 counts per second */
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

	/* 9000000/9000 = 1000 overflows per second - every 1ms one interrupt */
	/* SysTick interrupt every N clock pulses: set reload to N-1 */
	systick_set_reload(8999);

	systick_interrupt_enable();

	/* Start counting. */
	systick_counter_enable();
}
/******************************************************************************/
static void CO_CANconfigGPIO (void)
{

	/*TODO: make configuration depends on CANmodule 1/2*/

	/* Configure CAN pin: RX (input pull-up). */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
			GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_CAN_RX);
	gpio_set(GPIOA, GPIO_CAN_RX);

	/* Configure CAN pin: TX. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_CAN_TX);

	/*Remap for future */

	/*AFIO_MAPR |= AFIO_MAPR_CAN1_REMAP_PORTB;*/

	/*[> Configure CAN pin: RX (input pull-up). <]*/
	/*gpio_set_mode(GPIO_BANK_CAN1_PB_RX, GPIO_MODE_INPUT,*/
	/*GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_CAN1_PB_RX);*/
	/*gpio_set(GPIO_BANK_CAN1_PB_RX, GPIO_CAN1_PB_RX);*/

	/*[> Configure CAN pin: TX. <]*/
	/*gpio_set_mode(GPIO_BANK_CAN1_PB_TX, GPIO_MODE_OUTPUT_50_MHZ,*/
	/*GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_CAN1_PB_TX);*/

}
