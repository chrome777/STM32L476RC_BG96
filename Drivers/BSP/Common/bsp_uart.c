/**
  ******************************************************************************
  * @file    bsp_uart.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void USARTx_CharReception_Callback(void);
static void USARTx_Error_Callback(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function configures USARTx Instance.
  * @note   This function is used to :
  *         -1- Enable GPIO clock and configures the USART pins.
  *         -2- NVIC Configuration for USART interrupts.
  *         -3- Enable the USART peripheral clock and clock source.
  *         -4- Configure USART functional parameters.
  *         -5- Enable USART.
  * @param  None
  * @retval None
  */
//void Configure_USART(void)
void BSP_USARTx_Init(void)
{
	LL_GPIO_InitTypeDef 	GPIO_InitStructure;
  LL_USART_InitTypeDef 	USART_Initstruct;

  /* (1) Enable GPIO clock and configures the USART pins *********************/

  /* Enable the peripheral clock of GPIO Port */
  USARTx_TX_GPIO_CLK_ENABLE();
	USARTx_RX_GPIO_CLK_ENABLE();

  /* Configure Tx Pin */
	GPIO_InitStructure.Pin 				= USARTx_TX_PIN;
	GPIO_InitStructure.Mode 			= LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStructure.Alternate 	= USARTx_TX_GPIO_AF;
	GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStructure.Speed 			= LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Pull 			= LL_GPIO_PULL_NO;
	LL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

  /* Configure Rx Pin */
	GPIO_InitStructure.Pin 				= USARTx_RX_PIN;
	GPIO_InitStructure.Alternate 	= USARTx_RX_GPIO_AF;
  LL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

  /* (2) NVIC Configuration for USART interrupts */
  /*  - Set priority for USARTx_IRQn */
  /*  - Enable USARTx_IRQn */
  NVIC_SetPriority(USARTx_IRQn, 0);
  NVIC_EnableIRQ(USARTx_IRQn);

  /* (3) Enable USART peripheral clock and clock source ***********************/
  USARTx_CLK_ENABLE();

  /* Set clock source */
  USARTx_CLK_SOURCE();

  /* (4) Configure USART functional parameters ********************************/
  
  /* Disable USART prior modifying configuration registers */
  /* Note: Commented as corresponding to Reset value */
  // LL_USART_Disable(USARTx_INSTANCE);

  /* Set fields of initialization structure                   */
  /*  - BaudRate            : 115200                          */
  /*  - DataWidth           : LL_USART_DATAWIDTH_8B           */
  /*  - StopBits            : LL_USART_STOPBITS_1             */
  /*  - Parity              : LL_USART_PARITY_NONE            */
  /*  - TransferDirection   : LL_USART_DIRECTION_TX_RX        */
  /*  - HardwareFlowControl : LL_USART_HWCONTROL_NONE         */
  /*  - OverSampling        : LL_USART_OVERSAMPLING_16        */
  USART_Initstruct.BaudRate            = 115200;
  USART_Initstruct.DataWidth           = LL_USART_DATAWIDTH_8B;
  USART_Initstruct.StopBits            = LL_USART_STOPBITS_1;
  USART_Initstruct.Parity              = LL_USART_PARITY_NONE;
  USART_Initstruct.TransferDirection   = LL_USART_DIRECTION_TX_RX;
  USART_Initstruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_Initstruct.OverSampling        = LL_USART_OVERSAMPLING_16;
  
  /* Initialize USART instance according to parameters defined in initialization structure */
  LL_USART_Init(USARTx_INSTANCE, &USART_Initstruct);

  /* (5) Enable USART *********************************************************/
  LL_USART_Enable(USARTx_INSTANCE);

  /* Polling USART initialisation */
  while((!(LL_USART_IsActiveFlag_TEACK(USARTx_INSTANCE))) || (!(LL_USART_IsActiveFlag_REACK(USARTx_INSTANCE))))
  { 
  }

  /* Enable RXNE and Error interrupts */
  LL_USART_EnableIT_RXNE(USARTx_INSTANCE);
  LL_USART_EnableIT_ERROR(USARTx_INSTANCE);
}

int fputc(int ch, FILE *f)
{
//	while (!LL_USART_IsActiveFlag_TXE(USARTx_INSTANCE))
//	{}
//	/* Write character in Transmit Data register.
//	TXE flag is cleared by writing data in TDR register */
//	LL_USART_TransmitData8(USARTx_INSTANCE, ch);
//	
//	while (!LL_USART_IsActiveFlag_TC(USARTx_INSTANCE))
//	{}
//	return ch;
		return 0;
}

int BSP_USART_Write(USART_TypeDef *USARTx, char *buf, uint16_t len)
{
	uint16_t i;

	for(i=0; i<len; i++)
	{
		/* Wait for TXE flag to be raised */
		while (!LL_USART_IsActiveFlag_TXE(USARTx))
		{}
		/* Write character in Transmit Data register.
		TXE flag is cleared by writing data in TDR register */
		LL_USART_TransmitData8(USARTx, buf[i]);
	}

	/* Wait for TC flag to be raised for last char */
	while (!LL_USART_IsActiveFlag_TC(USARTx))
	{}
	return 0;
}

/******************************************************************************/
/*                 STM32L4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l4xx.s).                                               */
/******************************************************************************/

/**
  * Brief   This function handles USARTx Instance interrupt request.
  * Param   None
  * Retval  None
  */
void USARTx_IRQHandler(void)
{
  /* Check RXNE flag value in ISR register */
  if(LL_USART_IsActiveFlag_RXNE(USARTx_INSTANCE) && LL_USART_IsEnabledIT_RXNE(USARTx_INSTANCE))
  {
    /* RXNE flag will be cleared by reading of RDR register (done in call) */
    /* Call function in charge of handling Character reception */
    USARTx_CharReception_Callback();
  }
  else
  {
    /* Call Error function */
    USARTx_Error_Callback();
  }
}

/**
  * @brief  Function called from USART IRQ Handler when RXNE flag is set
  *         Function is in charge of reading character received on USART RX line.
  * @param  USARTx USART Instance
  * @retval None
  */
void USARTx_CharReception_Callback(void)
{
	__IO uint8_t received_char;
	
	received_char = LL_USART_ReceiveData8(USARTx_INSTANCE);
}

/**
  * @brief  Function called in case of error detected in USART IT Handler
  * @param   USARTx USART Instance
  * @retval None
  */
void USARTx_Error_Callback(void)
{
	/* Error handling example :
	 - Read USART SR register to identify flag that leads to IT raising
	 - Perform corresponding error handling treatment according to flag
	*/
//	if(LL_USART_IsActiveFlag_FE(USARTx_INSTANCE))
//	{
//		LL_USART_ClearFlag_FE(USARTx_INSTANCE);
//		printf("USART FE ERROR\r\n");
//	}
//	if(LL_USART_IsActiveFlag_NE(USARTx_INSTANCE))
//	{
//		LL_USART_ClearFlag_NE(USARTx_INSTANCE);
//		printf("USART NE ERROR\r\n");
//	}
//	if(LL_USART_IsActiveFlag_ORE(USARTx_INSTANCE))
//	{
//		LL_USART_ClearFlag_ORE(USARTx_INSTANCE);
//		printf("USART ORE ERROR\r\n");
//	}
//		if(LL_USART_IsActiveFlag_TEACK(USARTx_INSTANCE))
//	{
//		printf("USART TEACK ERROR\r\n");
//	}
//	if(LL_USART_IsActiveFlag_REACK(USARTx_INSTANCE))
//	{
//		printf("USART REACK ERROR\r\n");
//	}
//	if(LL_USART_IsActiveFlag_PE(USARTx_INSTANCE))
//	{
//		LL_USART_ClearFlag_PE(USARTx_INSTANCE);
//		printf("USART PE ERROR\r\n");
//	}
	if(LL_USART_IsActiveFlag_FE(USARTx_INSTANCE))
	{
		LL_USART_ClearFlag_FE(USARTx_INSTANCE);
		printf("USARTx FE ERROR\r\n");
	}
//	if(LL_USART_IsActiveFlag_NE(USARTx_INSTANCE))
//	{
//		printf("USART NE ERROR\r\n");
//	}
	if(LL_USART_IsActiveFlag_ORE(USARTx_INSTANCE))
	{
		LL_USART_ClearFlag_ORE(USARTx_INSTANCE);
		printf("USART ORE ERROR\r\n");
	}
	if(LL_USART_IsActiveFlag_IDLE(USARTx_INSTANCE))
	{
		LL_USART_ClearFlag_IDLE(USARTx_INSTANCE);
		printf("USART IDLE ERROR\r\n");
	}
//	if(LL_USART_IsActiveFlag_TC(USARTx_INSTANCE))
//	{
//		LL_USART_ClearFlag_TC(USARTx_INSTANCE);
//		printf("USART TC ERROR\r\n");
//	}
	if(LL_USART_IsActiveFlag_TXE(USARTx_INSTANCE))
	{
		printf("USART TXE ERROR\r\n");
	}
//	if(LL_USART_IsActiveFlag_LBD(USARTx_INSTANCE))
//	{
//		LL_USART_ClearFlag_LBD(USARTx_INSTANCE);
//		printf("USART CMF ERROR\r\n");
//	}
//	if(LL_USART_IsActiveFlag_nCTS(USARTx_INSTANCE))
//	{
//		printf("USART nCTS ERROR\r\n");
//	}
//	if(LL_USART_IsActiveFlag_CTS(USARTx_INSTANCE))
//	{
//		printf("USART CTS ERROR\r\n");
//	}
//	if(LL_USART_IsActiveFlag_RTO(USARTx_INSTANCE))
//	{
//		LL_USART_ClearFlag_RTO(USARTx_INSTANCE);
//		printf("USART RTO ERROR\r\n");
//	}
	if(LL_USART_IsActiveFlag_EOB(USARTx_INSTANCE))
	{
		LL_USART_ClearFlag_EOB(USARTx_INSTANCE);
		printf("USART EOB ERROR\r\n");
	}
//	if(LL_USART_IsActiveFlag_ABRE(USARTx_INSTANCE))
//	{
//		printf("USART ABRE ERROR\r\n");
//	}
//	if(LL_USART_IsActiveFlag_ABR(USARTx_INSTANCE))
//	{
//		printf("USART ABR ERROR\r\n");
//	}
//	if(LL_USART_IsActiveFlag_BUSY(USARTx_INSTANCE))
//	{
//		printf("USART BUSY ERROR\r\n");
//	}
	if(LL_USART_IsActiveFlag_CM(USARTx_INSTANCE))
	{
		LL_USART_ClearFlag_CM(USARTx_INSTANCE);
		printf("USART CMF ERROR\r\n");
	}
//	if(LL_USART_IsActiveFlag_SBK(USARTx_INSTANCE))
//	{
//		printf("USART SBK ERROR\r\n");
//	}
//	if(LL_USART_IsActiveFlag_RWU(USARTx_INSTANCE))
//	{
//		printf("USART RWU ERROR\r\n");
//	}
//	if(LL_USART_IsActiveFlag_WKUP(USARTx_INSTANCE))
//	{
//		printf("USART WKUP ERROR\r\n");
//	}
	if(LL_USART_IsActiveFlag_TEACK(USARTx_INSTANCE))
	{
		printf("USART TEACK ERROR\r\n");
	}
	if(LL_USART_IsActiveFlag_REACK(USARTx_INSTANCE))
	{
		printf("USART REACK ERROR\r\n");
	}
	LL_USART_ReceiveData8(USARTx_INSTANCE);
}