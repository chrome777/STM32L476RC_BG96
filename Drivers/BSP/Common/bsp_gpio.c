/**
  ******************************************************************************
  * @file    stm32f7xx_nucleo_144.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   This file provides set of firmware functions to manage:
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
  
/* Includes ------------------------------------------------------------------*/
#include "bsp_gpio.h"

#include "bsp_tim.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM32F7XX_NUCLEO_144
  * @{
  */   
    
/** @addtogroup STM32F7XX_NUCLEO_144_LOW_LEVEL 
  * @brief This file provides set of firmware functions to manage Leds and push-button
  *        available on STM32F7xx-Nucleo Kit from STMicroelectronics.
  * @{
  */ 

/** @defgroup STM32F7XX_NUCLEO_144_LOW_LEVEL_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32F7XX_NUCLEO_144_LOW_LEVEL_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32F7XX_NUCLEO_144_LOW_LEVEL_Private_Variables
  * @{
  */ 
GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT};

const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN, LED3_PIN};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT}; 
const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN}; 
const uint8_t BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn};

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured. 
  *   This parameter can be one of following parameters:
  *     @arg  LED1
  *     @arg  LED2
  *     @arg  LED3
  * @retval None
  */
void BSP_LED_Init(Led_TypeDef Led)
{
	/* Enable the GPIO_LED Clock */
	LEDx_GPIO_CLK_ENABLE(Led);

	/* Configure IO in output push-pull mode to drive external LED2 */
	LL_GPIO_SetPinMode(GPIO_PORT[Led], GPIO_PIN[Led], LL_GPIO_MODE_OUTPUT);
//	/* Reset value is LL_GPIO_OUTPUT_PUSHPULL */
//	LL_GPIO_SetPinOutputType(GPIO_PORT[Led], GPIO_PIN[Led], LL_GPIO_OUTPUT_PUSHPULL);
//	/* Reset value is LL_GPIO_SPEED_FREQ_LOW */
//	LL_GPIO_SetPinSpeed(GPIO_PORT[Led], GPIO_PIN[Led], LL_GPIO_SPEED_FREQ_LOW);
//	/* Reset value is LL_GPIO_PULL_NO */
//	LL_GPIO_SetPinPull(GPIO_PORT[Led], GPIO_PIN[Led], LL_GPIO_PULL_NO);
}

/**
  * @brief  DeInit LEDs.
  * @param  Led: LED to be de-init. 
  *   This parameter can be one of the following values:
  *     @arg  LED1
  *     @arg  LED2
  *     @arg  LED3
  * @note Led DeInit does not disable the GPIO clock nor disable the Mfx 
  * @retval None
  */
void BSP_LED_DeInit(Led_TypeDef Led)
{

}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED2
  * @retval None
  */
void BSP_LED_On(Led_TypeDef Led)
{
	LL_GPIO_ResetOutputPin(GPIO_PORT[Led], GPIO_PIN[Led]);
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg  LED1
  *     @arg  LED2
  *     @arg  LED3
  * @retval None
  */
void BSP_LED_Off(Led_TypeDef Led)
{
	LL_GPIO_SetOutputPin(GPIO_PORT[Led], GPIO_PIN[Led]);
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  *   This parameter can be one of following parameters:
  *     @arg  LED1
  *     @arg  LED2
  *     @arg  LED3 
  * @retval None
  */
void BSP_LED_Toggle(Led_TypeDef Led)
{
	LL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);
}

/**
  * @brief  Set LED1 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void BSP_LED_Blinking(Led_TypeDef Led, uint32_t Period)
{
	/* Toggle LED1 in an infinite loop */
	while (1)
	{
		LL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);  
		LL_mDelay(Period);
	}
}

/**
  * @brief  Configures Button GPIO and EXTI Line.
  * @param  Button: Specifies the Button to be configured.
  *   This parameter should be: BUTTON_USER
  * @param  ButtonMode: Specifies Button mode.
  *   This parameter can be one of following parameters:   
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO 
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                            generation capability  
  * @retval None
  */
void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
	/* Enable the BUTTON Clock */
	USER_BUTTON_GPIO_CLK_ENABLE();

	if(ButtonMode == BUTTON_MODE_GPIO)
	{
		/* Configure Button pin as input */
		LL_GPIO_SetPinMode(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_MODE_INPUT);
		LL_GPIO_SetPinPull(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_PULL_NO);
	}

	if(ButtonMode == BUTTON_MODE_EXTI)
	{
		/* Configure GPIO for BUTTON */
		LL_GPIO_SetPinMode(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_MODE_INPUT);
		LL_GPIO_SetPinPull(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_PULL_UP);

		/* Connect External Line to the GPIO*/
		USER_BUTTON_SYSCFG_SET_EXTI();

		/* Enable a rising trigger EXTI_Line15_10 Interrupt */
		USER_BUTTON_EXTI_LINE_ENABLE();
		USER_BUTTON_EXTI_FALLING_TRIG_ENABLE();

		/* Configure NVIC for USER_BUTTON_EXTI_IRQn */
		NVIC_SetPriority(USER_BUTTON_EXTI_IRQn, 3);
		NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn); 
	}
}

/**
  * @brief  Push Button DeInit.
  * @param  Button: Button to be configured
  *   This parameter should be: BUTTON_USER
  * @note PB DeInit does not disable the GPIO clock
  * @retval None
  */
void BSP_PB_DeInit(Button_TypeDef Button)
{

}

/**
  * @brief  Returns the selected Button state.
  * @param  Button: Specifies the Button to be checked.
  *   This parameter should be: BUTTON_USER  
  * @retval The Button GPIO pin value.
  */
uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
	return LL_GPIO_IsInputPinSet(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/******************************************************************************/
/*                 STM32L4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles external line 15_10 interrupt request.
  * @param  None
  * @retval None
  */
void USER_BUTTON_IRQHANDLER(void)
{
	/* Manage Flags */
	if(LL_EXTI_IsActiveFlag_0_31(USER_BUTTON_EXTI_LINE) != RESET)
	{
		/* Clear EXTI flag */
		LL_EXTI_ClearFlag_0_31(USER_BUTTON_EXTI_LINE);

		/* Handle user button press in dedicated function */
		UserButton_Callback();
	}
}

/**
  * @brief  Function to manage Button push
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
	/* Turn LED1 Off on User button press (allow to restart sequence) */
	BSP_LED_Toggle(LED1);
}
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
