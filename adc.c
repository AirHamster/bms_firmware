#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include "includes/adc.h"
#include "includes/gpio.h"
#include "includes/defines.h"
#include "includes/usart.h"
void adc_init(void)
{
	int i;

	rcc_periph_clock_enable(RCC_ADC1);

	/* Make sure the ADC doesn't run during config. */
	adc_power_off(ADC1);

	/* We configure everything for one single conversion. */
	adc_disable_scan_mode(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_disable_external_trigger_regular(ADC1);
	adc_set_right_aligned(ADC1);
	/* We want to read the temperature sensor, so we have to enable it. */
	adc_enable_temperature_sensor();
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);

	adc_power_on(ADC1);

	/* Wait for ADC starting up. */
	for (i = 0; i < 800000; i++)    /* Wait a bit. */
		__asm__("nop");

	adc_reset_calibration(ADC1);
	adc_calibrate(ADC1);

	rcc_periph_clock_enable(RCC_ADC2);

	/* Make sure the ADC doesn't run during config. */
	adc_power_off(ADC2);

	/* We configure everything for one single conversion. */
	adc_disable_scan_mode(ADC2);
	adc_set_single_conversion_mode(ADC2);
	adc_disable_external_trigger_regular(ADC2);
	adc_set_right_aligned(ADC2);
	/* We want to read the temperature sensor, so we have to enable it. */
	/*adc_enable_temperature_sensor();*/
	adc_set_sample_time_on_all_channels(ADC2, ADC_SMPR_SMP_28DOT5CYC);

	adc_power_on(ADC2);

	/* Wait for ADC starting up. */
	for (i = 0; i < 800000; i++)    /* Wait a bit. */
		__asm__("nop");

	adc_reset_calibration(ADC2);
	adc_calibrate(ADC2);
}
void adc_get(void)
{
	gpio_set(RED_LED_PORT, RED_LED);
	uint16_t temperature = 0;
	/*
	 * Start the conversion directly (not trigger mode).
	 */
	adc_start_conversion_direct(ADC2);

	/* Wait for end of conversion. */
	while (!(ADC_SR(ADC2) & ADC_SR_EOC));

	temperature = ADC_DR(ADC2);
	usart_send_byte(USART1, (temperature >> 8) & 0xFF);
	usart_send_byte(USART1, temperature & 0xFF);

}

void adc_get_temperature(void)
{
	gpio_set(RED_LED_PORT, RED_LED);
	uint16_t temperature = 0;
	/*
	 * Start the conversion directly (not trigger mode).
	 */
	adc_start_conversion_direct(ADC1);

	/* Wait for end of conversion. */
	while (!(ADC_SR(ADC1) & ADC_SR_EOC));

	temperature = ADC_DR(ADC1);

	/*
	 * That's actually not the real temperature - you have to compute it
	 * as described in the datasheet.
	 */
	my_usart_print_int(USART1, temperature);
}
