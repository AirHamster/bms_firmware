static void timer_setup(void)
{
	/* Set up the timer TIM2 for injected sampling */
	uint32_t timer;

	timer   = TIM2;
	rcc_periph_clock_enable(RCC_TIM2);

	/* Time Base configuration */
    rcc_periph_reset_pulse(RST_TIM2);
    timer_set_mode(timer, TIM_CR1_CKD_CK_INT,
	    TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_period(timer, 0xFF);
    timer_set_prescaler(timer, 0x8);
    timer_set_clock_division(timer, 0x0);
    /* Generate TRGO on every update. */
    timer_set_master_mode(timer, TIM_CR2_MMS_UPDATE);
    timer_enable_counter(timer);
} 
