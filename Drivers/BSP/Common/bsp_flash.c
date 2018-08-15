/**
  ******************************************************************************
  * @file    bsp_flash.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_flash.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t BSP_Flash_GetPage(uint32_t Addr)
{
	uint32_t page = 0;

	if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
	{
		/* Bank 1 */
		page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
	}
	else
	{
		/* Bank 2 */
		page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
	}

	return page;
}

/**
  * @brief  Gets the bank of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The bank of a given address
  */
static uint32_t BSP_Flash_GetBank(uint32_t Addr)
{
	uint32_t bank = 0;
  
	if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
	{
		/* No Bank swap */
		if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
		{
			bank = FLASH_BANK_1;
		}
		else
		{
			bank = FLASH_BANK_2;
		}
	}
	else
	{
		/* Bank swap */
		if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
		{
			bank = FLASH_BANK_2;
		}
		else
		{
			bank = FLASH_BANK_1;
		}
	}
  
	return bank;
}

static HAL_StatusTypeDef BSP_Flash_Erase(uint32_t Addr)
{
	/*Variable used for Erase procedure*/
	uint32_t FirstPage = 0, NbOfPages = 0, BankNumber = 0;
	uint32_t PAGEError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;

	/* Erase the user Flash area
	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	
	/* Clear OPTVERR bit set on virgin samples */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	
	/* Get the 1st page to erase */
	FirstPage = BSP_Flash_GetPage(FLASH_START_ADDR);
	
	/* Get the number of pages to erase from 1st page */
	NbOfPages = BSP_Flash_GetPage(FLASH_END_ADDR) - FirstPage + 1;
	
	/* Get the bank */
	BankNumber = BSP_Flash_GetBank(FLASH_START_ADDR);
	
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks       = BankNumber;
	EraseInitStruct.Page        = FirstPage;
	EraseInitStruct.NbPages     = NbOfPages;

	/* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	 you have to make sure that these data are rewritten before they are accessed during code
	 execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	 DCRST and ICRST bits in the FLASH_CR register. */
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		/*
		  Error occurred while page erase.
		  User can add here some code to deal with this error.
		  PAGEError will contain the faulty page and then to know the code error on this page,
		  user can call function 'HAL_FLASH_GetError()'
		*/
		return HAL_ERROR;
	}
	return HAL_OK;
}

void BSP_Flash_WriteByte(uint32_t addr, uint8_t *buf, uint32_t len)
{
	uint32_t i;

	__disable_irq();
	
	/* Unlock the Flash to enable the flash control register access ***********/
	HAL_FLASH_Unlock();

	/* Erase the user Flash area
	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	if( BSP_Flash_Erase(addr) == HAL_OK )
	{
		for(i=0; i<len; i++)
		{
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, (uint64_t)buf[i]) == HAL_OK)
			{
				addr += 8;
			}
		}
	}
	
	/* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	
	__enable_irq();
}

void BSP_Flash_WriteWord(uint32_t addr, uint32_t *buf, uint32_t len)
{
	uint32_t i;

	__disable_irq();
	
	/* Unlock the Flash to enable the flash control register access ***********/
	HAL_FLASH_Unlock();
	
	if( BSP_Flash_Erase(addr) == HAL_OK )
	{
		for(i=0; i<len; i++)
		{
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, (uint64_t)buf[i]) == HAL_OK)
			{
				addr = addr + 8;
			}
		}
	}
	
	/* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	
	__enable_irq();
}

void BSP_Flash_WriteDoubleWord(uint32_t addr, uint64_t *buf, uint32_t len)
{
	uint32_t i;

	__disable_irq();
	
	/* Unlock the Flash to enable the flash control register access ***********/
	HAL_FLASH_Unlock();

	/* Erase the user Flash area
	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	if( BSP_Flash_Erase(addr) == HAL_OK )
	{
		for(i=0; i<(len/8+1); i++)
		{
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, buf[i]) == HAL_OK)
			{
				addr = addr + 8;
			}
		}
	}
	
	/* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	
	__enable_irq();
}

void BSP_Flash_ReadByte(uint32_t addr, uint8_t *buf, uint32_t len)
{
	while (len--)
	{
		*buf = *(uint8_t *)addr;
		addr += 1;
		buf++;
	}
}

void BSP_Flash_ReadWord(uint32_t addr, uint32_t *buf, uint32_t len)
{
	while (len--)
	{
		*buf = *(uint32_t *)addr;
		addr += 4;
		buf++;
	}
}

void BSP_Flash_ReadDoubleWord(uint32_t addr, uint64_t *buf, uint32_t len)
{
	while (len--)
	{
		*buf = *(uint64_t *)addr;
		addr += 8;
		buf++;
	}
}