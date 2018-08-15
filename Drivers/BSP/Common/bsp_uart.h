/**
  ******************************************************************************
  * @file    bsp_uart.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_UART_H_
#define __BSP_UART_H_


/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Exported types ------------------------------------------------------------*/
#define RING_BUFFER_SIZE								(256 * 2)

typedef enum
{
	RING_RECV_IDLE = 0,
	RING_RECV_NEW  = 1,
	RING_RECV_OVER = 2
}RingRecvStatusTypeDef;

typedef struct
{
  uint8_t  data[RING_BUFFER_SIZE];
  uint16_t tail; 				
  uint16_t head;
	RingRecvStatusTypeDef status;
  uint8_t  timeout;
}RingBuffer_t;

/* Exported constants --------------------------------------------------------*/
/* USART2 instance is used. (TX on PA.02, RX on PA.03) */
#define USARTx_INSTANCE               USART2
#define USARTx_CLK_ENABLE()           LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2)
#define USARTx_CLK_SOURCE()           LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1)
#define USARTx_IRQn                   USART2_IRQn
#define USARTx_IRQHandler             USART2_IRQHandler

#define USARTx_TX_GPIO_CLK_ENABLE()		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define USARTx_TX_PIN                 LL_GPIO_PIN_2
#define USARTx_TX_GPIO_PORT           GPIOA
#define USARTx_TX_GPIO_AF       			LL_GPIO_AF_7

#define USARTx_RX_GPIO_CLK_ENABLE()		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define USARTx_RX_PIN                 LL_GPIO_PIN_3
#define USARTx_RX_GPIO_PORT           GPIOA
#define USARTx_RX_GPIO_AF       			LL_GPIO_AF_7

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

extern void BSP_USARTx_Init(void);


#endif
