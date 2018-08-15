/**
  ******************************************************************************
  * @file    bsp_tim.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_TIM_H_
#define __BSP_TIM_H_


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void BSP_TIM7_TimeBase_Init(uint32_t uwPrescalerValue, uint32_t Period);
extern void BSP_TIM7_TimeBase_Start(void);
extern void BSP_TIM7_TimeBase_Stop(void);

extern void TIM7_IRQHandler(void);
static void TimerUpdate_Callback(void);


#endif
