/*************************************************************************
* ARM LOGGING LIBRARY
* 
* USES STM ARM TIM2 (16 BIT) IN UPCOUNTING MODE TO PROVIDE TIMESTAMPING
* DEBUG FUNCTIONS . BASED ON PREPROCESSOR DEFINE (SEGGER_RTT / SWO), THE 
* OUTPUT IS PRINTED EITHER USING SWO(SWD) OR SEGGER RTT
*
* FOR TIMING, TIMER2 RUNS OFF (PCLK1 * APB2 PRESCALER) WITH PRESCALER 1
* SO TIMER FREQUENCY = 
*
* MARCH 24 2017
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
*
* REFERENCES
* ***********************************************************************/

#include "ARM_STM32_LOGGING.h"

static volatile uint32_t OVERFLOW_COUNT;
static uint32_t PCLK1_FREQUENCY;
static uint8_t APB1_PRESCALER;

//TIMER2 UPDATE EVENT INTERRUPT HANDLER
void TIM2_IRQHandler(void)
{
	//INCREAMENT OVERFLOW COUNT
	OVERFLOW_COUNT += 1;
	
	//CLEAR INTERRUPT PENDING BIT
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

void ARM_STM32_LOGGING_Start_Timekeeping(void)
{
	//START TIMEKEEPING
	//TIMER-2 IN SIMPLE UPCOUNTING MODE WITH FREQUENCY = PCLK1
	//SETUP TIMER2 (APB1)
	
	OVERFLOW_COUNT = 0;
	
	//SET PCLK1 FREQUENCY VARIABLE FOR TIME CALCULATIONS
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	PCLK1_FREQUENCY = clocks.PCLK1_Frequency;

	//GET APB2 PRESCALER
	if(clocks.HCLK_Frequency == clocks.PCLK1_Frequency)
	{
		APB1_PRESCALER = 1;
	}
	else
	{
		APB1_PRESCALER = clocks.HCLK_Frequency / clocks.PCLK1_Frequency;
	}
	
	//ENABLE INTERRUPT FOR TIMER2
	NVIC_InitTypeDef nvic;
	
	nvic.NVIC_IRQChannel = TIM2_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitTypeDef timer;
	timer.TIM_ClockDivision = TIM_CKD_DIV1;
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	timer.TIM_Period = 0xFFFF;
	timer.TIM_Prescaler = 0x0000;
	
	//SET TIMER2 COUNTER TO 0
	TIM_SetCounter(TIM2, 0);
	
	//ENABLE UPDATE EVENT INTERRUPT ON TIMER2
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	//INITIALIZE TIMER2 PARAMETERS AND START TIMER
	TIM_TimeBaseInit(TIM2, &timer);
	
	//CLEAR THE TIM2 UPDATE EVENT WHICH IS SET AS SOON
	//AS THE TIMER UPDATE EVENT IS ENABLES (SPL BUG)
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
	//ENABLE TIM2 UPDATE EVENT INTERRUPT
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	//ENABLE TIMER
	TIM_Cmd(TIM2, ENABLE);
}

void ARM_STM32_LOGGING_Stop_Timekeeping(void)
{
	//STOP TIMEKEEPING
	//STOP TIMER
	//RETURN THE TOTAL TIMER COUNT (INCLUDING OVERFLOWS)
	
	//STOP TIMER2
	TIM_Cmd(TIM2, DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
}

void ARM_STM32_LOGGING_Print_Pretty(void)
{
	//PRINT THE SUPPLIED TIMEKEEPING STAMP IN PRETTY
	//FORMAT OVER THE DEBUG TERMINAL
	
	uint32_t total_ticks = TIM_GetCounter(TIM2);
	total_ticks = (OVERFLOW_COUNT * 0xFFFF) + total_ticks;
	
	uint32_t timer_frequency;
	if(APB1_PRESCALER == 1)
	{
		timer_frequency = PCLK1_FREQUENCY;
	}
	else
	{
		timer_frequency = PCLK1_FREQUENCY * 2;
	}
	
	float time_elapsed = (float)total_ticks / timer_frequency;
	
	#ifdef SEGGER_RTT
		//NOTE : SEGGER RTT PRINTF DOES NOT SUPPORT FLOAT PRINTING
		//SO NEED A WORKAROUND
		uint32_t part_int = (uint32_t)time_elapsed;
		//FLOAT PART WITH 4 DIGITS OF PRESCISION
		uint32_t part_dec = (time_elapsed - part_int) * 1000;
		SEGGER_RTT_printf(0, "Ticks = %d | Time Elapsed = %d.%d seconds\n", total_ticks, part_int, part_dec);
	#endif
	
	#ifdef SWO
	
	#endif
}
