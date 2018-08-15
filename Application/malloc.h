/**
  ******************************************************************************
  * @file    malloc.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MALLOC_H_
#define __MALLOC_H_


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdint.h"
#include "string.h"

/* Exported types ------------------------------------------------------------*/
#define SRAMBANK	1  				// Support the definition of the SRAM number

// 内存管理控制器
struct _m_mallco_dev
{
	void (*init)(uint8_t);			// Memory init
	uint8_t  (*perused)(uint8_t);	// Memory utilization
	uint8_t  *membase[SRAMBANK];	// Memory pool
	uint32_t *memmap[SRAMBANK];		// Memory status table
	uint8_t  memrdy[SRAMBANK];		// Memory management state
};

/* Exported constants --------------------------------------------------------*/

#define SRAMIN		0	// Internal memory pool
#define SRAMEX		1  	// External memory pool
#define SRAMCCM		2  	// CCM memory pool(This part of SRAM is only CPU accessible)

// Internal memory pool
#define MEM1_BLOCK_SIZE			64
#define MEM1_MAX_SIZE			  30*1024
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE


/* Exported macro ------------------------------------------------------------*/
extern struct _m_mallco_dev mallco_dev;							// defined in malloc.c

/* Exported functions ------------------------------------------------------- */
static uint8_t USR_Mem_Perused(uint8_t memx);

extern void SRAM_Malloc_Init(uint8_t memx);
extern void SRAM_Free(uint8_t memx, void *ptr);
extern void *SRAM_Malloc(uint8_t memx, uint32_t size);
extern void *SRAM_Realloc(uint8_t memx, void *ptr, uint32_t size);



#endif


