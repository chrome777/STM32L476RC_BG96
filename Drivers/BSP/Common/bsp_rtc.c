/**
  ******************************************************************************
  * @file    bsp_rtc.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_rtc.h"

#include <time.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Defines related to Clock configuration */
/* Uncomment to enable the adaquate Clock Source */
#define RTC_CLOCK_SOURCE_LSE
/*#define RTC_CLOCK_SOURCE_LSE*/

#ifdef RTC_CLOCK_SOURCE_LSI
	#define RTC_ASYNCH_PREDIV		0x7F
	#define RTC_SYNCH_PREDIV		0xF9
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
	#define RTC_ASYNCH_PREDIV		0x7F
	#define RTC_SYNCH_PREDIV		0x00FF
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  RTC Configuration
  *         RTC Clocked by LSI (see HAL_RTC_MspInit)
  * @param  None
  * @retval None
  */
int8_t BSP_RTC_Init(void)
{
	/* Configure RTC prescaler and RTC data registers */
	/* RTC configured as follow:
	  - Hour Format    = Format 12
	  - Asynch Prediv  = Value according to source clock
	  - Synch Prediv   = Value according to source clock
	  - OutPut         = Output Disable
	  - OutPutPolarity = High Polarity
	  - OutPutType     = Open Drain */
	__HAL_RTC_RESET_HANDLE_STATE(&hrtc);
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat     = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
  hrtc.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
  hrtc.Init.OutPut         = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
//	__HAL_RTC_RESET_HANDLE_STATE(&hrtc);
//	hrtc.Instance = RTC;
//	hrtc.Init.HourFormat			= RTC_HOURFORMAT_24;
//	hrtc.Init.AsynchPrediv		= RTC_ASYNCH_PREDIV;
//	hrtc.Init.SynchPrediv			= RTC_SYNCH_PREDIV;
//  hrtc.Init.OutPut 					= RTC_OUTPUT_DISABLE;
//  hrtc.Init.OutPutRemap 		= RTC_OUTPUT_REMAP_NONE;
//  hrtc.Init.OutPutPolarity	= RTC_OUTPUT_POLARITY_HIGH;
//  hrtc.Init.OutPutType 			= RTC_OUTPUT_TYPE_OPENDRAIN;

	if(HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		return -1;
	}
	return 0;
}

/**
  * @brief RTC MSP Initialization 
  *        This function configures the hardware resources used in this example
  * @param hrtc: RTC handle pointer
  * 
  * @note  Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select 
  *        the RTC clock source; in this case the Backup domain will be reset in  
  *        order to modify the RTC Clock source, as consequence RTC registers (including 
  *        the backup registers) and RCC_BDCR register are set to their reset values.
  *             
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  
  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  /* To change the source clock of the RTC feature (LSE, LSI), You have to:
     - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
     - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
       configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
       __HAL_RCC_BACKUPRESET_RELEASE().
     - Configure the needed RTC clock source */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  /*##-2- Configure LSE/LSI as RTC clock source ###############################*/
#ifdef RTC_CLOCK_SOURCE_LSE
  
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
		printf("HAL_RCC_OscConfig LSE ERR\r\n");
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    printf("HAL_RCC_OscConfig LSE ERR\r\n");
  }
#elif defined (RTC_CLOCK_SOURCE_LSI)  
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    printf("HAL_RCC_OscConfig LSI ERR\r\n");
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    printf("HAL_RCC_OscConfig LSI ERR\r\n");
  }
#else
#error Please select the RTC Clock source inside the main.h file
#endif /*RTC_CLOCK_SOURCE_LSE*/
  
  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */ 
  __HAL_RCC_RTC_ENABLE(); 
  
  /*##-4- Configure the NVIC for RTC TimeStamp ###############################*/
  HAL_NVIC_SetPriority(TAMP_STAMP_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(TAMP_STAMP_IRQn);
}
/**
  * @brief RTC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
   __HAL_RCC_RTC_DISABLE();
}

/*
#define RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
#define RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
#define RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)
*/
// cnt:Auto reload value reduced to 0, resulting in interrupts
uint8_t BSP_RTC_WakeUp_Init(uint32_t wksel, uint16_t cnt)
{
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);	/* Reset Internal Wake up flag */
	
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, cnt, wksel);
	
	HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x02, 0x02);
	HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
	
	return 0;
}

/**
  * @brief  Configure the current time and date and activate timestamp.
  * @param  timebuf:// "2017/08/08,05:55;50"
  * @retval None
  */
void BSP_RTC_TimeStampWrite(uint8_t *time_buf)
{
	RTC_DateTypeDef sdatestructure;
	RTC_TimeTypeDef stimestructure;
	struct tm tm_time;
	time_t tmp;
	
	sscanf((char *)time_buf, "%02d/%02d/%02d,%02d:%02d:%02d", &tm_time.tm_year,
														&tm_time.tm_mon,
														&tm_time.tm_mday,
														&tm_time.tm_hour,
														&tm_time.tm_min,
														&tm_time.tm_sec);
	tm_time.tm_year += 100;
	tm_time.tm_mon -= 1;
	tm_time.tm_isdst = -1;
	tmp = mktime(&tm_time)+ 3600*8;			// +8time zone
	tm_time = *gmtime(&tmp);

//	sdatestructure.Year = tm_time.tm_year - 100;
//	sdatestructure.Month = tm_time.tm_mon + 1;
//	sdatestructure.Date = tm_time.tm_mday;
//	stimestructure.Hours = tm_time.tm_hour;
//	stimestructure.Minutes = tm_time.tm_min;
//	stimestructure.Seconds = tm_time.tm_sec;
	
	/*##-1- Configure the Time Stamp peripheral ################################*/
  /*  RTC TimeStamp generation: TimeStamp Rising Edge on PC.13 Pin */
  HAL_RTCEx_SetTimeStamp_IT(&hrtc, RTC_TIMESTAMPEDGE_RISING, RTC_TIMESTAMPPIN_DEFAULT);

  /*##-2- Configure the Date #################################################*/
  /* Set Date: Monday April 14th 2014 */
  sdatestructure.Year    = tm_time.tm_year - 100;
  sdatestructure.Month   = tm_time.tm_mon + 1;
  sdatestructure.Date    = tm_time.tm_mday;
  sdatestructure.WeekDay = tm_time.tm_wday;
  
  if(HAL_RTC_SetDate(&hrtc, &sdatestructure, RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    printf("RTC SetDate Error\r\n");
  } 
  
  /*##-3- Configure the Time #################################################*/
  /* Set Time: 08:10:00 */
  stimestructure.Hours          = tm_time.tm_hour;
  stimestructure.Minutes        = tm_time.tm_min;
  stimestructure.Seconds        = tm_time.tm_sec;
  stimestructure.SubSeconds     = 0x00;
  stimestructure.TimeFormat     = RTC_HOURFORMAT12_PM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if(HAL_RTC_SetTime(&hrtc, &stimestructure, RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    printf("RTC SetTime Error\r\n"); 
  }
}

/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
void BSP_RTC_TimeStampRead(uint8_t *time_buf)
{
	RTC_DateTypeDef hrtc_date_get;
	RTC_TimeTypeDef hrtc_time_get;

//	HAL_RTCEx_GetTimeStamp(&hrtc, &hrtc_time_get, &hrtc_date_get, RTC_FORMAT_BIN);
	
	/* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &hrtc_time_get, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&hrtc, &hrtc_date_get, RTC_FORMAT_BIN);
	/* Display time Format : yy-mm-dd hh:mm:ss */
	sprintf((char *)time_buf, "20%.2d-%.2d-%.2d %.2d:%.2d:%.2d",hrtc_date_get.Year,
													hrtc_date_get.Month,
													hrtc_date_get.Date,
													hrtc_time_get.Hours,
													hrtc_time_get.Minutes,
													hrtc_time_get.Seconds);
	
	/* Display date Format : yy-mm-dd */
	printf("time_stamp: %.2d-%.2d-%.2d,", 2000 + hrtc_date_get.Year, hrtc_date_get.Month, hrtc_date_get.Date);
	/* Display time Format : hh:mm:ss */
	printf("%.2d:%.2d:%.2d\r\n", hrtc_time_get.Hours, hrtc_time_get.Minutes, hrtc_time_get.Seconds);
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * Brief   This function handles RTC interrupt request.
  * Param   None
  * Retval  None
  */
void RTC_IRQHandler(void)
{

}

/**
  * @brief  This function handles Tamper interrupt request.
  * @param  None
  * @retval None
  */
void TAMP_STAMP_IRQHandler(void)
{
	HAL_RTCEx_TamperTimeStampIRQHandler(&hrtc);
}

/**
  * @brief  Timestamp callback 
  * @param  hrtc : hrtc handle
  * @retval None
  */
void HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc)
{
	RTC_DateTypeDef hrtc_date;
	RTC_TimeTypeDef hrtc_time;

	HAL_RTCEx_GetTimeStamp(hrtc, &hrtc_time, &hrtc_date, RTC_FORMAT_BIN);
}

/**
  * Brief   This function handles RTC WKUP interrupt request.
  * Param   None
  * Retval  None
  */
void RTC_WKUP_IRQHandler(void)
{
	HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc); 
}