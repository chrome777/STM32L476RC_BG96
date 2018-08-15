/**
  ******************************************************************************
  * @file    utils.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEV_UTILS_H_
#define __DEV_UTILS_H_


/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void my_itoa(int n, char *a);
void hexdump(const unsigned char *buf, unsigned int num);
unsigned char prvucMBBIN2CHAR(unsigned char ucByte);
unsigned short usMBCRC16( unsigned char * pucFrame, unsigned short  usLen );
extern char* memstr(char* full_data, int full_data_len, char* substr);


#endif

