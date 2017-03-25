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

#ifndef _ARM_STM32_LOGGING_H_
#define _ARM_STM32_LOGGING_H_

#include <stdint.h>

#ifdef STM32F072
	#include "stm32f07x.h"
	#include "stm32f10x_misc.h"
#endif

#ifdef STM32F103
	#include "stm32f10x.h"
	#include "stm32f10x_rcc.h"
	#include "stm32f10x_tim.h"
	#include "misc.h"
#endif

#ifdef SEGGER_RTT
	#include "SEGGER_RTT.h"
#endif

static volatile uint32_t OVERFLOW_COUNT;
static uint32_t PCLK1_FREQUENCY;
static uint8_t APB1_PRESCALER;

void TIM2_IRQHandler(void);
void ARM_STM32_LOGGING_Start_Timekeeping(void);
void ARM_STM32_LOGGING_Stop_Timekeeping(void);
void ARM_STM32_LOGGING_Print_Pretty(void);

#endif
