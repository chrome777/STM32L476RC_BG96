/** 
  ******************************************************************************
  * @file    stm32f7xx_nucleo_144.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   This file contains definitions for:
  *          - LEDs and push-button available on STM32F7XX-Nucleo-144 Kit 
  *            from STMicroelectronics
  *          - LCD, joystick and microSD available on Adafruit 1.8" TFT LCD 
  *            shield (reference ID 802)
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************  
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_GPIO_H_
#define __BSP_GPIO_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

   
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32F7XX_NUCLEO_144
  * @{
  */

/** @addtogroup STM32F7XX_NUCLEO_144_LOW_LEVEL
  * @{
  */ 

/** @defgroup STM32F7XX_NUCLEO_144_LOW_LEVEL_Exported_Types
  * @{
  */
typedef enum 
{
	LED1 = 0,
	LED_GREEN = LED1,
	LED2 = 1,
	LED_BLUE = LED2,
	LED3 = 2,
	LED_RED = LED3
}Led_TypeDef;

typedef enum 
{  
	BUTTON_USER = 0,
	/* Alias */
	BUTTON_KEY = BUTTON_USER
}Button_TypeDef;

typedef enum 
{  
	BUTTON_MODE_GPIO = 0,
	BUTTON_MODE_EXTI = 1
}ButtonMode_TypeDef;

/**
  * @}
  */ 

/** @defgroup STM32F7XX_NUCLEO_144_LOW_LEVEL_Exported_Constants
  * @{
  */ 

/** @addtogroup STM32L4XX_NUCLEO_144_LOW_LEVEL_LED
  * @{
  */
#define LEDn                                    3

#define LED1_PIN                                GPIO_PIN_2
#define LED1_GPIO_PORT                          GPIOD
#define LED1_GPIO_CLK_ENABLE()                  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD)
#define LED1_GPIO_CLK_DISABLE()                 LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOD)

#define LED2_PIN                                GPIO_PIN_3
#define LED2_GPIO_PORT                          GPIOD
#define LED2_GPIO_CLK_ENABLE()                  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD)
#define LED2_GPIO_CLK_DISABLE()                 LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD)

#define LED3_PIN                                GPIO_PIN_4
#define LED3_GPIO_PORT                          GPIOD
#define LED3_GPIO_CLK_ENABLE()                  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD)
#define LED3_GPIO_CLK_DISABLE()                 LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD)  

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)   do { if((__INDEX__) == 0) {LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);} else\
                                                                    {LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);   }} while(0)
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)  do { if((__INDEX__) == 0) {LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);} else\
                                                                    {LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);   }} while(0)
                                                                      

/** @addtogroup STM32L4XX_NUCLEO_144_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                                 1

																		/**
  * @brief Key push-button
  */
#define USER_BUTTON_PIN                         LL_GPIO_PIN_0
#define USER_BUTTON_GPIO_PORT                   GPIOA
#define USER_BUTTON_GPIO_CLK_ENABLE()           LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define USER_BUTTON_GPIO_CLK_DISABLE()					LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define USER_BUTTON_EXTI_LINE                   LL_EXTI_LINE_0
#define USER_BUTTON_EXTI_IRQn                   EXTI0_IRQn
#define USER_BUTTON_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_SYSCFG_SET_EXTI()           do {                                                                     \
                                                  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);                  \
                                                  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE13);  \
                                                } while(0)
#define USER_BUTTON_IRQHANDLER                  EXTI0_IRQHandler

/* Aliases */
#define KEY_BUTTON_PIN                       USER_BUTTON_PIN
#define KEY_BUTTON_GPIO_PORT                 USER_BUTTON_GPIO_PORT
#define KEY_BUTTON_GPIO_CLK_ENABLE()         USER_BUTTON_GPIO_CLK_ENABLE()
#define KEY_BUTTON_GPIO_CLK_DISABLE()        USER_BUTTON_GPIO_CLK_DISABLE()
#define KEY_BUTTON_EXTI_LINE                 USER_BUTTON_EXTI_LINE
#define KEY_BUTTON_EXTI_IRQn                 USER_BUTTON_EXTI_IRQn

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup STM32F7XX_NUCLEO_144_LOW_LEVEL_Exported_Macros
  * @{
  */  
/**
  * @}
  */ 

/** @defgroup STM32F7XX_NUCLEO_144_LOW_LEVEL_Exported_Functions
  * @{
  */
void		BSP_LED_Init(Led_TypeDef Led);
void		BSP_LED_DeInit(Led_TypeDef Led);
void		BSP_LED_On(Led_TypeDef Led);
void		BSP_LED_Off(Led_TypeDef Led);
void		BSP_LED_Toggle(Led_TypeDef Led);
void		BSP_LED_Blinking(Led_TypeDef Led, uint32_t Period);
void		BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
void		BSP_PB_DeInit(Button_TypeDef Button);
uint32_t	BSP_PB_GetState(Button_TypeDef Button);


/* IRQ Handler treatment functions */
void USER_BUTTON_IRQHANDLER(void);
void UserButton_Callback(void);

  
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F7XX_NUCLEO_144_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
