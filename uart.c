
/************************************************************
 *	FILE NAME:	usart.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/nvic.h>
#include "includes/usart.h"
#include "includes/adc.h"
#include "includes/can.h"
#include "includes/defines.h"

char help_msg[] = "Plazma probe controller\n Usage:\n    start - start measurements\n    stop - finish measurements\n    set <voltage> - probe voltage setup\n    native - non-formated output\n    ascii - output in ascii presentation\n";
char stringa[] = "STRING\n";
uint8_t resiever[50], rec_len = 0;
struct usart
{
	uint32_t *global_pointer;
	uint8_t *data_pointer;
	uint8_t data1;
	uint8_t data2;
	uint8_t data3;
	uint8_t data4;
	uint8_t lenth;
	uint8_t byte_counter;
	unsigned int busy:1;
};
	struct usart usart1;
void usart_init(void)
{
	/* Enable clocks for GPIO port A (for GPIO_USART1_TX) and USART1. */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
	/* Enable the USART1 interrupt. */
	nvic_enable_irq(NVIC_USART1_IRQ);

	/* Setup GPIO pin GPIO_USART1_RE_TX on GPIO port B for transmit. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	/* Setup GPIO pin GPIO_USART1_RE_RX on GPIO port B for receive. */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
			GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);

	/* Enable USART1 Receive interrupt. */
	USART_CR1(USART1) |= USART_CR1_RXNEIE;

	/* Finally enable the USART. */
	usart_enable(USART1);
}

void usart1_isr(void)
{
	uint8_t tmp;

	//Check if we were called because of RXNE. 
	if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
			((USART_SR(USART1) & USART_SR_RXNE) != 0)) {

		/* Retrieve the data from the peripheral. */
		tmp = usart_recv(USART1);
		if (tmp == '\n')
		{
			resiever[rec_len++] = 0;	/* Make null-terminated string */
			process_command(resiever);
			rec_len = 0;
		}else{
			resiever[rec_len++] = tmp;
		}

					/*USART_CR1(USART1) &= ~USART_CR1_RXNEIE;*/
	}
	//Check if we were called because of TXE. 
	if (((USART_CR1(USART1) & USART_CR1_TXEIE) != 0) &&
			((USART_SR(USART1) & USART_SR_TXE) != 0)) {
		/*Check the count of non-sended words*/	
		if (usart1.lenth != 0){
			/*send bytes until it will be send*/
			if (usart1.byte_counter-- != 0){
				usart_send(USART1, *usart1.data_pointer++);
			}else{
				if(--usart1.lenth !=0){
					/*Reconfig usarts pointers and byte array*/
					usart1.byte_counter = 3;
					usart1.global_pointer++;
					usart1.data1 = (*usart1.global_pointer >> 24) & 0xff;
					usart1.data2 = (*usart1.global_pointer >> 16) & 0xff;
					usart1.data3 = (*usart1.global_pointer >> 8) & 0xff;
					usart1.data4 = (*usart1.global_pointer) & 0xff;
					usart1.data_pointer = &usart1.data1;
					usart_send(USART1, *usart1.data_pointer++);
				}else{
					//Disable the TXE interrupt as we don't need it anymore. 
					USART_CR1(USART1) &= ~USART_CR1_TXEIE;
					usart1.busy = 0;
				}
			}

		}else{
			//Disable the TXE interrupt as we don't need it anymore. 
			USART_CR1(USART1) &= ~USART_CR1_TXEIE;
			usart1.busy = 0;
		}
	}
}

void usart_send_string(uint32_t USART, uint8_t *BufferPtr, uint16_t Length )
{

	while ( Length != 0 )
	{
		usart_send_blocking(USART, *BufferPtr);
		BufferPtr++;
		Length--;
	}

	return;
}

void usart_send_32(uint32_t USART, uint32_t *data, uint8_t lenth)
{
	while (usart1.busy);
	usart1.busy = 1;	
	//Divide 32bit to 8bit
	usart1.data1 = (*data >> 24) & 0xff;
	usart1.data2 = (*data >> 16) & 0xff;
	usart1.data3 = (*data >> 8) & 0xff;
	usart1.data4 = (*data) & 0xff;
	usart1.lenth = lenth;
	usart1.byte_counter = 4;
	usart1.global_pointer = data;
	usart1.data_pointer = &usart1.data1;	//
	usart_send_blocking(USART, *usart1.data_pointer++);
	usart1.byte_counter--;
	//Enable TxE interrupt
	USART_CR1(USART) |= USART_CR1_TXEIE;
}

void process_command(char *cmd)
{
	uint16_t t;
	if(strncmp(cmd, "LED", 3) == 0)
	{
		gpio_toggle(GREEN_LED_PORT, GREEN_LED);
		usart_send_byte(USART1, 'l');
		/*UART0_send("z\n", 2);*/
		//gpio_set(OP_AMP_PORT, OP_AMP_PIN);
		/*led_set(LED2);*/
		//	timer0_start();
	}    

	if(strncmp(cmd, "can", 3) == 0)
	{
		can_send_test(0, 0);
		/*gpio_toggle(GREEN_LED_PORT, GREEN_LED);*/
		/*UART0_send("\nStarted\n", 9);*/
	}    

	if(strncmp(cmd, "ONE", 3) == 0)
	{
		can_send_test(1, 0);
		/*gpio_toggle(GREEN_LED_PORT, GREEN_LED);*/
		/*UART0_send("\nStarted\n", 9);*/
	}
	if(strncmp(cmd, "TWO", 3) == 0)

	{
		can_send_test(2, 0);
	/*gpio_toggle(GREEN_LED_PORT, GREEN_LED);*/
		/*UART0_send("\nStarted\n", 9);*/
	}

	if(strncmp(cmd, "THREE", 5) == 0)

	{
		can_send_test(3, 0);
	/*gpio_toggle(GREEN_LED_PORT, GREEN_LED);*/
		/*UART0_send("\nStarted\n", 9);*/
	}
	if(strncmp(cmd, "TEMP", 4) == 0)
	{
		adc_get_temperature();
	/*gpio_toggle(GREEN_LED_PORT, GREEN_LED);*/
		/*UART0_send("\nStarted\n", 9);*/
	}    
	if(strncmp(cmd, "ADC", 3) == 0)
	{
	t = adc_get();
	usart_send_byte(USART1, (t >> 8) & 0xFF);
	usart_send_byte(USART1, t & 0xFF);
	/*gpio_toggle(GREEN_LED_PORT, GREEN_LED);*/
		/*UART0_send("\nStarted\n", 9);*/
	}    
	/* Turn off amplifier */
	if(strncmp(cmd, "stop", 4) == 0)
	{
		//UART0_send("\nStopped\n", 9);
	}
	/* Manual  */
	if(strncmp(cmd, "help", 4) == 0)
	{
		usart_send_string(USART1, help_msg, sizeof(help_msg)-1);
		/*usart_send_string(USART1, stringa, 7);*/
		/*usart_send_string(USART1, "AAAAA\n", strlen("AAAAA\n"));*/
	/*usart_send_string(USART1, "Hello \n", strlen("Hello \n"));*/
	}
	/* Switching between output value presentation */
}

void usart_send_data(uint32_t USART, uint32_t *data, uint8_t lenth)
{
	while (usart1.busy);
	usart1.busy = 1;	
	usart1.lenth = lenth;
	usart1.byte_counter = 4;
	usart1.global_pointer = data;
	usart1.data_pointer = &usart1.data1;	//
	usart_send_blocking(USART, *usart1.data_pointer++);
	usart1.byte_counter--;
	//Enable TxE interrupt
	USART_CR1(USART) |= USART_CR1_TXEIE;
}

void usart_send_byte(uint32_t USART, uint8_t data)
{

	while (usart1.busy);
	usart_send_blocking(USART, data);
}

double atof(const char *s)
{
	// This function stolen from either Rolf Neugebauer or Andrew Tolmach. 
	// Probably Rolf.
	double a = 0.0;
	int e = 0;
	int c;
	uint8_t neg_flag = 0;
	if ((c = *s++) == '-')
	{
		neg_flag = 1;
	}
	while ((c = *s++) != '\0' && isdigit(c)) {
		a = a*10.0 + (c - '0');
	}
	if (c == '.') {
		while ((c = *s++) != '\0' && isdigit(c)) {
			a = a*10.0 + (c - '0');
			e = e-1;
		}
	}
	if (c == 'e' || c == 'E') {
		int sign = 1;
		int i = 0;
		c = *s++;
		if (c == '+')
			c = *s++;
		else if (c == '-') {
			c = *s++;
			sign = -1;
		}
		while (isdigit(c)) {
			i = i*10 + (c - '0');
			c = *s++;
		}
		e += i*sign;
	}
	while (e > 0) {
		a *= 10.0;
		e--;
	}
	while (e < 0) {
		a *= 0.1;
		e++;
	}
	if (neg_flag == 1)
		a = a*(-1);
	return a;
}
void my_usart_print_int(uint32_t usart, int value)
{
	int8_t i;
	uint8_t nr_digits = 0;
	char buffer[25];

	if (value < 0) {
		usart_send_blocking(usart, '-');
		value = value * -1;
	}

	while (value > 0) {
		buffer[nr_digits++] = "0123456789"[value % 10];
		value /= 10;
	}

	for (i = (nr_digits - 1); i >= 0; i--) {
		usart_send_blocking(usart, buffer[i]);
	}

	usart_send_blocking(usart, '\r');
}
