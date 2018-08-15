/**
  ******************************************************************************
  * @file    malloc.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "malloc.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

// Memory pool (4 bytes aligned)
#pragma pack(4)
uint8_t mem1base[MEM1_MAX_SIZE] @(0X10000000);
#pragma pack()
// Memory management table
uint32_t mem1mapbase[MEM1_ALLOC_TABLE_SIZE] @(0X10000000+MEM1_MAX_SIZE);

// Memory management parameter
const uint32_t memtblsize[SRAMBANK] = {MEM1_ALLOC_TABLE_SIZE};	// Memory table size
const uint32_t memblksize[SRAMBANK] = {MEM1_BLOCK_SIZE};		// Memory block size
const uint32_t memsize[SRAMBANK] = {MEM1_MAX_SIZE};				// Memory size

// Memory management controller
struct _m_mallco_dev mallco_dev=
{
	SRAM_Malloc_Init,
	USR_Mem_Perused,    // Memory utilization
	mem1base,           // Memory poll
	mem1mapbase,        // Memory management status table
	0,                  // Memory management is not ready
};

/* Private function prototypes -----------------------------------------------*/
static void USR_Mem_Copy(void *des, void *src, uint32_t n);
static void USR_Mem_Set(void *s, uint8_t c, uint32_t count);

static uint32_t USR_Mem_Malloc(uint8_t memx, uint32_t size);
static uint8_t USR_Mem_Free(uint8_t memx, uint32_t offset);

/* Private functions ---------------------------------------------------------*/


// Memory copy
// *des		: Destination address
// *src		: source address
// n		: The length of memory to be copied (in bytes)
void USR_Mem_Copy(void *des, void *src, uint32_t n)
{
	uint8_t *xdes = des;
	uint8_t *xsrc = src;
	while(n--) *xdes++ = *xsrc++;
}

// Memory set
// *s		: Memory first address
// c 		: The value to set
// count	: The length of memory to be Set (in bytes)
void USR_Mem_Set(void*s, uint8_t c, uint32_t count)
{
	uint8_t *xs = s;
	while(count--) *xs ++= c;
}

// Memory utilization
// memx 	: Memory block
// return 	: utilization (0~100)
uint8_t USR_Mem_Perused(uint8_t memx)
{  
  uint32_t used=0;
  uint32_t i;
  for(i=0; i<memtblsize[memx]; i++)
  {
    if(mallco_dev.memmap[memx][i])
      used++;
  }
  return (used*100)/(memtblsize[memx]);
} 

// Memory malloc
// memx		: Memory block
// size		: The size of the memory to allocate (bytes)
// return	: 0XFFFFFFFF,fail; else, offset address
uint32_t USR_Mem_Malloc(uint8_t memx, uint32_t size)
{
  signed long offset=0;
  uint16_t nmemb;			// The number of memory blocks
  uint16_t cmemb=0;		// Continuous empty memory blocks
  uint32_t i;

  if(!mallco_dev.memrdy[memx])						// Memory uninitialized
    mallco_dev.init(memx);	
  if(size == 0)
    return 0XFFFFFFFF;								// No allocation required
  nmemb = size/memblksize[memx];						// Get assigned contiguous memory blocks
  if(size%memblksize[memx])
    nmemb++;
  for(offset=memtblsize[memx]-1; offset>=0; offset--)	// Search the entire memory control area
  {
		if(!mallco_dev.memmap[memx][offset])
			cmemb++;									// Continuous empty memory blocks 
		else
			cmemb = 0;
		if(cmemb == nmemb)								// A continuous nmemb empty memory block was found
		{
      for(i=0; i<nmemb; i++)  					// Callout memory block non empty
      {
        mallco_dev.memmap[memx][offset+i] = nmemb;
      }
      return (offset*memblksize[memx]);			// Return offset address
		}
  }
  return 0XFFFFFFFF;									// No memory blocks found to match the allocation condition
}

// Memory free
// memx		: Memory block
// offset	: Offset of the memory
// return 	: 0,sucess	1,fail
uint8_t USR_Mem_Free(uint8_t memx, uint32_t offset)
{
	int i;
	
	if(!mallco_dev.memrdy[memx])					// Memory uninitialized
	{
		mallco_dev.init(memx);						// Memory init
		return 1;
	}
	if(offset < memsize[memx])						// Offset in the memory pool
	{
		int index = offset/memblksize[memx];		// The number of memory blocks in the offset 
		int nmemb = mallco_dev.memmap[memx][index];	// Memory block number
		for(i=0; i<nmemb; i++)						// Memory block cleared
		{
			mallco_dev.memmap[memx][index+i] = 0;
		}
		return 0;
	}
	else
		return 2;									// Offset out of range
}

// Memory management initialization
// memx		: Memory block
void SRAM_Malloc_Init(uint8_t memx)
{
	USR_Mem_Set(mallco_dev.memmap[memx], 0, memtblsize[memx]*2);	// Memory status table cleared
	USR_Mem_Set(mallco_dev.membase[memx], 0, memsize[memx]);		// Memory pool, all data cleared
	mallco_dev.memrdy[memx] = 1;									// Memory management initialization OK 
}

// Malloc memory
// memx		: Memory block
// size		: Memory size (bytes)
// return 	: The first address of the allocated memory
void *SRAM_Malloc(uint8_t memx, uint32_t size)
{
	uint32_t offset;
	offset = USR_Mem_Malloc(memx, size);
	if(offset == 0XFFFFFFFF)
		return NULL;
	else
		return (void*)((uint32_t)mallco_dev.membase[memx]+offset);
}

// Free Memory
// memx		: 
// ptr		: The first address of the memory
void SRAM_Free(uint8_t memx, void *ptr)
{
	uint32_t offset;
  if(ptr == NULL)
		return;
 	else
		offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase[memx];
  USR_Mem_Free(memx, offset);
}

// Reassign memory
// memx		: Memory block
// *ptr		: The first address of the old memory 
// size		: The size of the memory to allocate (bytes)
// return	: The first address of the new allocated memory 
void *SRAM_Realloc(uint8_t memx, void *ptr, uint32_t size)
{
  uint32_t offset;
  offset = USR_Mem_Malloc(memx,size);
  if(offset == 0XFFFFFFFF)
    return NULL;
  else
  {
    USR_Mem_Copy((void*)((uint32_t)mallco_dev.membase[memx]+offset), ptr, size);	// Copy old memory to new memory
    SRAM_Free(memx, ptr);															// Freeing old memory
    return (void*)((uint32_t)mallco_dev.membase[memx] + offset);					// Returns the new memory first address
  }
}
