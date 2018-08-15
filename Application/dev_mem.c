/**
  ******************************************************************************
  * @file    k_mem.c
  * @author  MCD Application Team   
  * @brief   Utilities for file handling
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright(c) 2017 STMicroelectronics International N.V. 
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
#include "dev_mem.h"
#include "string.h"


/** @addtogroup CORE
  * @{
  */

/** @defgroup KERNEL_MEMORY
  * @brief Kernel memory routines
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MEM_BASE                     0x10000000
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if   (defined ( __CC_ARM ))
mem_TypeDef memory_pool __attribute__((at(MEM_BASE)));
#elif (defined (__ICCARM__))
#pragma location = MEM_BASE
__no_init mem_TypeDef memory_pool;
#elif defined   (  __GNUC__  )
mem_TypeDef memory_pool __attribute__((section(".ExtRAMData")));
#elif defined ( __TASKING__ )
mem_TypeDef memory_pool __at(MEM_BASE);  
#endif

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function Initialize a memory pool for private allocator.
  * @param  None
  * @retval None
  */
void BSP_MemInit(void)
{
  memset(&memory_pool, 0, sizeof(mem_TypeDef));
  memory_pool.mallocBase = MEM_BASE + sizeof(mem_TypeDef);
}

/**
  * @brief  This function implement a simple memory management algorithm (First fit).
  * @param  size : Requested memory size.
  * @retval Pointer to the allocated region.
  */
void *BSP_malloc(size_t size)
{
  uint32_t index = 0, PageCount = 0, start = 0;

  if (size > 0)
  {
    /* look for the first contiguous memory that meet requested size. */
    while ( index < (MAX_PAGE_NUMBER))
    {
      if (memory_pool.PageTable[index++] == 0)
      {
        PageCount++;
        if (size <= (PageCount * SIZE_OF_PAGE))
        {
          /* Set memory region state to allocated */ /* Save size */
          for (index = 0; index < PageCount; index++)
          {
            memory_pool.PageTable[start + index] = PageCount;
          }
          
          /* return pointer to allocated region. */
          return (void *)((memory_pool.mallocBase + (start  << OFFSET_OF_PAGE)) + sizeof(uint32_t));
        }
      }
      else
      {
        start = index;
        PageCount = 0;
      }
    }
  }
  return NULL;
}

/**
  * @brief  Free previously allocated memory
  * @param  Pointer to the allocated region.
  * @retval None.
  */
void BSP_free(void * p)
{
  uint8_t index = 0;
  uint8_t counter = 0, start = 0;
  /* Handle NULL pointers */
  if (p == NULL)
    return;

  /* Find start Index */
  start = ((((uint32_t)p - sizeof(uint32_t)) - memory_pool.mallocBase) >> OFFSET_OF_PAGE);
  /* Retrieve segment size */
  counter = memory_pool.PageTable[start];
  /* Deallocate memory region */
  for (index = 0; index < counter; index++)
  {
    memory_pool.PageTable[start + index] = 0;
  }

  return;
}

/**
  * @}
  */

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
