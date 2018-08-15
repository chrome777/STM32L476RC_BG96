/**
  ******************************************************************************
  * @file    bsp_rtc.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_RTC_H_
#define __BSP_RTC_H_


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

extern int8_t BSP_RTC_Init(void);
extern uint8_t BSP_RTC_WakeUp_Init(uint32_t wksel, uint16_t cnt);
extern void BSP_RTC_TimeStampWrite(uint8_t *time_buf);
extern void BSP_RTC_TimeStampRead(uint8_t *time_buf);
void RTC_IRQHandler(void);
void RTC_WKUP_IRQHandler(void);


#endif
