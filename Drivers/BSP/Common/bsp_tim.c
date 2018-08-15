/**
  ******************************************************************************
  * @file    bsp_tim.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_tim.h"

#include "app_mx35.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Configures the timer as a time base.
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
void BSP_TIM7_TimeBase_Init(uint32_t uwPrescalerValue, uint32_t Period)
{
	LL_TIM_InitTypeDef TIM7_InitStructure;
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);
	
	TIM7_InitStructure.Autoreload				= Period-1;
	TIM7_InitStructure.Prescaler				= uwPrescalerValue-1;
	TIM7_InitStructure.ClockDivision		= LL_TIM_CLOCKDIVISION_DIV1;
	TIM7_InitStructure.CounterMode			= LL_TIM_COUNTERMODE_UP;
	LL_TIM_Init(TIM7, &TIM7_InitStructure);
	
	LL_TIM_ClearFlag_UPDATE(TIM7);
	/* Enable the update interrupt */
	LL_TIM_EnableIT_UPDATE(TIM7);

	/* Configure the NVIC to handle TIM6 update interrupt */
	NVIC_SetPriority(TIM7_IRQn, 0);
	NVIC_EnableIRQ(TIM7_IRQn);
	
//	/* Enable the update interrupt */
//	LL_TIM_EnableCounter(TIM7);
}

void BSP_TIM7_TimeBase_Start(void)
{
//	TIM7->CNT = 0;
	LL_TIM_SetCounter(TIM7, 0);
	/* Enable the update interrupt */
	LL_TIM_EnableCounter(TIM7);
}


void BSP_TIM7_TimeBase_Stop(void)
{
	/* Disable the update interrupt */
	LL_TIM_DisableCounter(TIM7);
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

///**
//* @brief  This function handles TIM7 update interrupt.
//* @param  None
//* @retval None
//*/
//void TIM6_DAC_IRQHandler(void)
//{
//	/* Check whether update interrupt is pending */
//	if(LL_TIM_IsActiveFlag_UPDATE(TIM6) == 1)
//	{
//		/* Clear the update interrupt flag*/
//		LL_TIM_ClearFlag_UPDATE(TIM6);
//		BSP_LED_Toggle(LED2);
//	}
//}

/**
* @brief  This function handles TIM7 update interrupt.
* @param  None
* @retval None
*/
void TIM7_IRQHandler(void)
{
	/* Check whether update interrupt is pending */
	if(LL_TIM_IsActiveFlag_UPDATE(TIM7))
	{
		/* Clear the update interrupt flag*/
		LL_TIM_ClearFlag_UPDATE(TIM7);
		MX35_Info.tim_flag = 1;
//		TimerUpdate_Callback();
	}
}

///**
//  * @brief  Timer update interrupt processing
//  * @param  None
//  * @retval None
//  */
//static void TimerUpdate_Callback(void)
//{
//	/* TIM7 update interrupt processing */
////	LL_TIM_DisableCounter(TIM7);
//	BSP_LED_Toggle(LED1);
////	MX35_Info.tim_flag = 1;
//}