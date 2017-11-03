 #include <libopencm3/stm32/adc.h>
 #include <libopencm3/stm32/rcc.h>
#include "includes/adc.h"
void adc_init(void)
{
	int i;
	rcc_periph_clock_enable(RCC_ADC1);

	/* Make sure the ADC doesn't run during config. */
	adc_power_off(ADC1);

	/* We configure everything for one single timer triggered injected conversion. */
	adc_disable_scan_mode(ADC1);
	adc_set_single_conversion_mode(ADC1);
	/* We can only use discontinuous mode on either the regular OR injected channels, not both */
	adc_disable_discontinuous_mode_regular(ADC1);
	adc_enable_discontinuous_mode_injected(ADC1);
	/* We want to start the injected conversion with the TIM2 TRGO */
	adc_enable_external_trigger_injected(ADC1,ADC_CR2_JEXTSEL_TIM2_TRGO);
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
}
