/**
  ******************************************************************************
  * @file    ug96_io.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the C2C IO operations.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UG96_IO__
#define __UG96_IO__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define UART_BG96_USE_DMA
#define UART_BG96_USE_DMA_TX
#define UART_BG96_USE_DMA_RX

/* This section can be used to tailor UART_BG96 instance used and associated
   resources */
#define UART_BG96                        USART3
#define UART_BG96_CLK_ENABLE()           __HAL_RCC_USART3_CLK_ENABLE();
#define UART_BG96_FORCE_RESET()          __HAL_RCC_USART3_FORCE_RESET()
#define UART_BG96_RELEASE_RESET()        __HAL_RCC_USART3_RELEASE_RESET()

/* Definition for UART_BG96's DMA */
#ifdef UART_BG96_USE_DMA_TX
	#define UART_BG96_DMA_TX_CLK_ENABLE()    __HAL_RCC_DMA1_CLK_ENABLE()
	#define UART_BG96_DMA_TX_CHANNEL         DMA1_Channel2
	#define UART_BG96_DMA_TX_REQUEST         DMA_REQUEST_2
	#define UART_BG96_DMA_TX_IRQn            DMA1_Channel2_IRQn
	#define UART_BG96_DMA_TX_IRQHandler      DMA1_Channel2_IRQHandler
#endif

#ifdef UART_BG96_USE_DMA_RX
	#define UART_BG96_DMA_RX_CLK_ENABLE()    __HAL_RCC_DMA1_CLK_ENABLE()
	#define UART_BG96_DMA_RX_CHANNEL         DMA1_Channel3
	#define UART_BG96_DMA_RX_REQUEST         DMA_REQUEST_2
	#define UART_BG96_DMA_RX_IRQn            DMA1_Channel3_IRQn
	#define UART_BG96_DMA_RX_IRQHandler      DMA1_Channel3_IRQHandler
#endif

/* Definition for UART_BG96 Pins */
#define UART_BG96_TX_PIN                 GPIO_PIN_10
#define UART_BG96_TX_GPIO_PORT           GPIOB
#define UART_BG96_TX_AF                  GPIO_AF7_USART3
#define UART_BG96_TX_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

#define UART_BG96_RX_PIN                 GPIO_PIN_11
#define UART_BG96_RX_GPIO_PORT           GPIOB
#define UART_BG96_RX_AF                  GPIO_AF7_USART3
#define UART_BG96_RX_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

/* Definition for UART_BG96's NVIC IRQ and IRQ Handlers */
#define UART_BG96_IRQn                   USART3_IRQn
#define UART_BG96_IRQHandler             USART3_IRQHandler

/* BG96 module Reset pin definitions */
#define BG96_RST_PIN                     GPIO_PIN_8
#define BG96_RST_GPIO_PORT               GPIOC
#define BG96_RST_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()

/* BG96 module PowerKey pin definitions */
#define BG96_PWR_PIN                     GPIO_PIN_9
#define BG96_PWR_GPIO_PORT               GPIOC
#define BG96_PWR_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()

/* BG96 module PowerKey pin definitions */
#define BG96_DTR_PIN                     GPIO_PIN_12
#define BG96_DTR_GPIO_PORT               GPIOB
#define BG96_DTR_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void BG96_HwResetAndPowerUp(void);

int8_t UART_BG96_Init(void);
int8_t UART_BG96_DeInit(void);
int8_t UART_BG96_SetBaudrate(uint32_t BaudRate);
void   UART_BG96_FlushBuffer(void);
int8_t UART_BG96_SendData(uint8_t* Buffer, uint16_t Length);
int8_t UART_BG96_ReceiveSingleData(uint8_t* pData);
int8_t UART_BG96_ReceiveData(uint8_t* Buffer, uint32_t Length);

#ifdef __cplusplus
}
#endif

#endif /* __BG96_IO__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
