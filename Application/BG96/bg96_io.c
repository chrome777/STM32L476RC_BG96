/**
  ******************************************************************************
  * @file    ug96_io.c
  * @author  MCD Application Team
  * @brief   This file implments the IO operations to deal with the C2C 
  *          module. It mainly Inits and Deinits the UART interface. Send and
  *          receive data over it.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2017 STMicroelectronics International N.V. 
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

/* Includes ------------------------------------------------------------------*/
#include "bg96_io.h"
#include "bg96_conf.h"

#include "stm32l4xx_ll_usart.h"

#include "string.h"

/* Private define ------------------------------------------------------------*/
/* intermediate buffer: filled by IRQ and retrieved by the UART_BG96_ReceiveSingleData()*/
/* by whatching real time, the difference between write_ptr and read ptr is often less then 50 */
#define RING_BUFFER_SIZE                                                   1500

/* Private typedef -----------------------------------------------------------*/
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
}RingBuffer_t;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RingBuffer_t BG96_RxBuffer;
UART_HandleTypeDef hUART_bg96;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * @param hUART_bg96: UART handle pointer
  * @retval None
  */
static void UART_BG96_MspInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
#ifdef UART_BG96_USE_DMA_TX
  static DMA_HandleTypeDef hdma_tx;
#endif
#ifdef UART_BG96_USE_DMA_RX
	static DMA_HandleTypeDef hdma_rx;
#endif
	
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  UART_BG96_TX_GPIO_CLK_ENABLE();
  UART_BG96_RX_GPIO_CLK_ENABLE();

  /* Enable UART_BG96 clock */
  UART_BG96_CLK_ENABLE(); 

  /* Enable DMA clock */
#ifdef UART_BG96_USE_DMA_TX
  UART_BG96_DMA_TX_CLK_ENABLE();
#endif
#ifdef UART_BG96_USE_DMA_RX
	UART_BG96_DMA_RX_CLK_ENABLE();
#endif

  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = UART_BG96_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = UART_BG96_TX_AF;

  HAL_GPIO_Init(UART_BG96_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = UART_BG96_RX_PIN;
  GPIO_InitStruct.Alternate = UART_BG96_RX_AF;
  
  HAL_GPIO_Init(UART_BG96_RX_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the NVIC for UART ########################################*/   
  HAL_NVIC_SetPriority(UART_BG96_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(UART_BG96_IRQn);
  
  /*##-4- Configure the DMA ##################################################*/
#ifdef UART_BG96_USE_DMA_TX
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = UART_BG96_DMA_TX_CHANNEL;
	hdma_tx.Init.Request             = UART_BG96_DMA_TX_REQUEST;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&hdma_tx);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(&hUART_bg96, hdmatx, hdma_tx);
#endif
#ifdef UART_BG96_USE_DMA_RX
  /* Configure the DMA handler for Transmission process */
  hdma_rx.Instance                 = UART_BG96_DMA_RX_CHANNEL;
	hdma_rx.Init.Request             = UART_BG96_DMA_RX_REQUEST;
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode                = DMA_CIRCULAR;
  hdma_rx.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&hdma_rx);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(&hUART_bg96, hdmarx, hdma_rx);
#endif

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (UART_BG96_TX) */
  HAL_NVIC_SetPriority(UART_BG96_DMA_TX_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(UART_BG96_DMA_TX_IRQn);
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param hUART_bg96: UART handle pointer
  * @retval None
  */
static void UART_BG96_MspDeInit(UART_HandleTypeDef *hUART_bg96)
{
#ifdef UART_BG96_USE_DMA_TX
  static DMA_HandleTypeDef hdma_tx;
#endif
#ifdef UART_BG96_USE_DMA_RX
	static DMA_HandleTypeDef hdma_rx;
#endif
  
  /*##-1- Reset peripherals ##################################################*/
  UART_BG96_FORCE_RESET();
  UART_BG96_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(UART_BG96_TX_GPIO_PORT, UART_BG96_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(UART_BG96_RX_GPIO_PORT, UART_BG96_RX_PIN);
  
  /*##-3- Disable the DMA Channels ###########################################*/
  /* De-Initialize the DMA Channel associated to transmission process */
  HAL_DMA_DeInit(&hdma_tx);

  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(UART_BG96_DMA_TX_IRQn);
}

/* Public functions ---------------------------------------------------------*/

/**
  * @brief Hw reset sequence and power up of the modem (it takes about 3 seconds)
  * @retval None
  */
void BG96_HwResetAndPowerUp(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStruct;  

  BG96_RST_GPIO_CLK_ENABLE();
  BG96_PWR_GPIO_CLK_ENABLE();
	
  /* RESET sequence LOW */
  GPIO_InitStruct.Pin       = BG96_RST_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(BG96_RST_GPIO_PORT, &GPIO_InitStruct);

  /* PWRKEY at least LOW during 100 ms */
  GPIO_InitStruct.Pin       = BG96_PWR_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(BG96_PWR_GPIO_PORT, &GPIO_InitStruct);

  /* POWER DOWN */
  HAL_GPIO_WritePin(BG96_PWR_GPIO_PORT, BG96_PWR_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BG96_RST_GPIO_PORT, BG96_RST_PIN, GPIO_PIN_SET);
  HAL_Delay(150);
  /* POWER UP */
  HAL_GPIO_WritePin(BG96_PWR_GPIO_PORT, BG96_PWR_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BG96_RST_GPIO_PORT, BG96_RST_PIN, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(BG96_PWR_GPIO_PORT, BG96_PWR_PIN,GPIO_PIN_SET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(BG96_PWR_GPIO_PORT, BG96_PWR_PIN,GPIO_PIN_RESET);
  /* Waits for Modem complete its booting procedure */
  HAL_Delay(1000);
}


/**
  * @brief  C2C IO Initalization.
  *         This function inits the UART interface to deal with the C2C,
  *         then starts asynchronous listening on the RX port.
  * @param None
  * @retval 0 on success, -1 otherwise.
  */
int8_t UART_BG96_Init(void)
{
  /* Set the C2C USART configuration parameters on MCU side */
  /* Attention: make sure the module uart is configured with the same values */
  hUART_bg96.Instance        = UART_BG96;
  hUART_bg96.Init.BaudRate   = BG96_DEFAULT_BAUDRATE;  
  hUART_bg96.Init.WordLength = UART_WORDLENGTH_8B;
  hUART_bg96.Init.StopBits   = UART_STOPBITS_1;
  hUART_bg96.Init.Parity     = UART_PARITY_NONE;
  hUART_bg96.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  hUART_bg96.Init.Mode       = UART_MODE_TX_RX;
  hUART_bg96.Init.OverSampling = UART_OVERSAMPLING_16;

  UART_BG96_MspInit();
  /* Configure the USART IP */
  if(HAL_UART_Init(&hUART_bg96) != HAL_OK)
  {
    return -1;
  }
 
  /* Once the C2C UART is initialized, start an asynchronous recursive
   listening. the HAL_UART_Receive_IT() call below will wait until one char is
   received to trigger the HAL_UART_RxCpltCallback(). The latter will recursively
   call the former to read another char.  */
  BG96_RxBuffer.head = 0;
  BG96_RxBuffer.tail = 0;

#ifdef UART_BG96_USE_DMA_RX
		HAL_UART_Receive_DMA(&hUART_bg96, BG96_RxBuffer.data, RING_BUFFER_SIZE);
	__HAL_UART_ENABLE_IT(&hUART_bg96, UART_IT_IDLE);
#else
  HAL_UART_Receive_IT(&hUART_bg96, (uint8_t *)&BG96_RxBuffer.data[BG96_RxBuffer.tail], 1);
#endif
	
  return 0;
}


/**
  * @brief  C2C IO Deinitialization.
  *         This function Deinits the UART interface of the C2C. When called
  *         the C2C commands can't be executed anymore.
  * @param  None.
  * @retval 0 on success, -1 otherwise.
  */
int8_t UART_BG96_DeInit(void)
{
  /* Reset USART configuration to default */
  HAL_UART_DeInit(&hUART_bg96);
  UART_BG96_MspDeInit(&hUART_bg96);
  
  return 0;
}


/**
  * @brief  C2C IO change baudrate
  *         To be used just in case the UG96_DEFAULT_BAUDRATE need to be changed
  *         This function has to be called after having changed the C2C module baudrate
  *         In order to do that the SMT32 Init shall be done at UG96_DEFAULT_BAUDRATE
  *         After C2C module baudrate is changed this function sets the STM32 baudrate accordingly
  * @param  None.
  * @retval 0 on success, -1 otherwise.
  */
int8_t UART_BG96_SetBaudrate(uint32_t BaudRate)
{
  HAL_UART_DeInit(&hUART_bg96);
  hUART_bg96.Init.BaudRate   = BaudRate;  
  if(HAL_UART_Init(&hUART_bg96) != HAL_OK)
  {
    return -1;
  }
  /* Once the C2C UART is initialized, start an asynchronous recursive
   listening. the HAL_UART_Receive_IT() call below will wait until one char is
   received to trigger the HAL_UART_RxCpltCallback(). The latter will recursively
   call the former to read another char.  */
  BG96_RxBuffer.head = 0;
  BG96_RxBuffer.tail = 0;
	
#ifdef UART_BG96_USE_DMA_RX
		HAL_UART_Receive_DMA(&hUART_bg96, BG96_RxBuffer.data, RING_BUFFER_SIZE);
	__HAL_UART_ENABLE_IT(&hUART_bg96, UART_IT_IDLE);
#else
  HAL_UART_Receive_IT(&hUART_bg96, (uint8_t *)&BG96_RxBuffer.data[BG96_RxBuffer.tail], 1);
#endif
	
  return 0;
}


/**
  * @brief  Flush Ring Buffer
  * @param  None
  * @retval None.
  */
void UART_BG96_FlushBuffer(void)
{
  memset(BG96_RxBuffer.data, 0, RING_BUFFER_SIZE);
  BG96_RxBuffer.head = BG96_RxBuffer.tail = 0;
}

/**
  * @brief  Send Data to the C2C module over the UART interface.
  *         This function allows sending data to the  C2C Module, the
  *         data can be either an AT command or raw data to send over 
  *         a pre-established C2C connection.
  * @param pData: data to send.
  * @param Length: the data length.
  * @retval 0 on success, -1 otherwise.
  */
int8_t UART_BG96_SendData(uint8_t* pData, uint16_t Length)
{
#ifdef UART_BG96_USE_DMA_TX
  if (HAL_UART_Transmit_DMA(&hUART_bg96, (uint8_t*)pData, Length) != HAL_OK)
#else 
	if (HAL_UART_Transmit(&hUART_bg96, (uint8_t*)pData, Length, 1000) != HAL_OK)
#endif
  {
     return -1;
  }

  return 0;
}

/**
  * @brief  Retrieve on Data from intermediate IT buffer 
  * @param pData: data to Receive.
  * @retval 0 data available, -1 no data to retrieve
  */
int8_t UART_BG96_ReceiveSingleData(uint8_t* pSingleData)
{
  /* Note: other possible implementation is to retrieve directly one data from UART buffer */
  /* without using the interrupt and the intermediate buffer */

  if(BG96_RxBuffer.head != BG96_RxBuffer.tail)
  {
    /* serial data available, so return data to user */
    *pSingleData = BG96_RxBuffer.data[BG96_RxBuffer.head++];
    
    /* check for ring buffer wrap */
    if (BG96_RxBuffer.head >= RING_BUFFER_SIZE)
    {
      /* ring buffer wrap, so reset head pointer to start of buffer */
      BG96_RxBuffer.head = 0;
    }
  }
  else
  {
		return -1;      
  }
    
  return 0;
}

/**
  * @brief  Retrieve on Data from intermediate IT buffer 
  * @param pData: data to Receive.
  * @param Length: the length of Receive data.
  * @retval available data length
  */
int8_t UART_BG96_ReceiveData(uint8_t* Buffer, uint32_t Length)
{
	uint32_t ReadData = 0;
  
	if(Length == 0)
	{
		return ReadData;
	}
	
	if(BG96_RxBuffer.status == RING_RECV_OVER)
	{
		while(BG96_RxBuffer.head != BG96_RxBuffer.tail)
		{
			/* serial data available, so return data to user */
			*Buffer++ = BG96_RxBuffer.data[BG96_RxBuffer.head++];
			ReadData++;
			
			/* check for ring buffer wrap */
			if (BG96_RxBuffer.head >= RING_BUFFER_SIZE)
			{
				/* Ring buffer wrap, so reset head pointer to start of buffer */
				BG96_RxBuffer.head = 0;
			}
		}
		BG96_RxBuffer.status = RING_RECV_IDLE;
	}

	return ReadData;
}

/**
  * @brief  Function called from USART IRQ Handler when RXNE flag is set
  *         Function is in charge of reading character received on USART RX line.
  * @param  USARTx USART Instance
  * @retval None
  */
void BG96_CharReception_Callback(void)
{
#ifdef UART_BG96_USE_DMA_RX
	if((__HAL_UART_GET_FLAG(&hUART_bg96, UART_FLAG_IDLE) != RESET))
	{
		__HAL_UART_CLEAR_IDLEFLAG(&hUART_bg96);
//		HAL_UART_DMAStop(&hUART_bg96);
		BG96_RxBuffer.tail = RING_BUFFER_SIZE - hUART_bg96.hdmarx->Instance->CNDTR;
		BG96_RxBuffer.status = RING_RECV_OVER;
//		HAL_UART_Receive_DMA(&hUART_bg96, BG96_RxBuffer.data, RING_BUFFER_SIZE);
	}
#else
	/* If ring buffer end is reached reset tail pointer to start of buffer */
  if(++BG96_RxBuffer.tail >= RING_BUFFER_SIZE)
  {
    BG96_RxBuffer.tail = 0;   
  }
  
  HAL_UART_Receive_IT(hUART_bg96, (uint8_t *)&BG96_RxBuffer.data[BG96_RxBuffer.tail], 1);
#endif
}

/**
  * @brief  Function called in case of error detected in USART IT Handler
  * @param   USARTx USART Instance
  * @retval None
  */
void BG96_Error_Callback(void)
{
	UART_BG96_DeInit();
	UART_BG96_Init();
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void UART_BG96_IRQHandler(void)
{
  /* Customize process using LL interface to improve the performance (exhaustive feature management not handled) */

  /* Check RXNE flag value in ISR register */
  if(LL_USART_IsActiveFlag_IDLE(UART_BG96) && LL_USART_IsEnabledIT_IDLE(UART_BG96))
  {
    /* RXNE flag will be cleared by reading of RDR register (done in call) */
    /* Call function in charge of handling Character reception */
    BG96_CharReception_Callback();
  }

  if(LL_USART_IsEnabledIT_ERROR(UART_BG96) && LL_USART_IsActiveFlag_NE(UART_BG96))
  {
    /* Call Error function */
    BG96_Error_Callback();
  }
}

#ifdef UART_BG96_USE_DMA_TX
/**
  * @brief  This function handles DMA interrupt request for Quectel modem
  * @param  None
  * @retval None
  */
void UART_BG96_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hUART_bg96.hdmatx);
}
#endif

#ifdef UART_BG96_USE_DMA_RX
/**
  * @brief  This function handles DMA interrupt request for Quectel modem
  * @param  None
  * @retval None
  */
void UART_BG96_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hUART_bg96.hdmarx);
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/