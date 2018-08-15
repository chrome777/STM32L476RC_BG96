/**
  ******************************************************************************
  * @file    app_bg96.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_BG96_H_
#define __APP_BG96_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdint.h"

/* Private define ------------------------------------------------------------*/
#define MAX_BUFFER_SIZE         512
#define MAX_AT_CMD_SIZE         256
#define AT_CMD_TIMEOUT					5000 	/* in ms */

#define AT_OK_STRING            "OK\r\n"
#define AT_IPD_OK_STRING        "OK\r\n\r\n"
#define AT_SEND_OK_STRING       "SEND OK\r\n"
#define AT_SEND_PROMPT_STRING   ">"
#define AT_ERROR_STRING         "ERROR\r\n"
#define AT_QIURC_STRING         "+QIURC:"
#define AT_RECV_STRING          "recv"
#define AT_NTP_UPDATE_STRING		"+QNTP:"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  BG96_FALSE         = 0,
  BG96_TRUE          = 1
} BG96_Boolean;

typedef enum
{ 
  BG96_OK                            = 0,
  BG96_ERROR                         = 1,
  BG96_BUSY                          = 2,
  BG96_ALREADY_CONNECTED             = 3,
  BG96_CONNECTION_CLOSED             = 4,
  BG96_TIMEOUT                       = 5,
  BG96_IO_ERROR                      = 6,
} BG96_StatusTypeDef;

typedef enum
{
  BG96_GOT_IP_STATUS       = 1,
  BG96_CONNECTED_STATUS    = 2,
  BG96_DISCONNECTED_STATUS = 3,
} BG96_ConnectionStatusTypeDef;

typedef enum
{
  BG96_TCP_CONNECTION = 0,
  BG96_UDP_CONNECTION = 1,
} BG96_ConnectionModeTypeDef;

typedef struct
{
	uint8_t nband;
	uint8_t cfun;
	uint8_t cimi[16];
	uint8_t csq[7];
	uint8_t cgatt;
	uint8_t cereg;
	uint8_t cscon;
	uint8_t time[25];
	int8_t  sock;
	uint8_t ipaddr[15];
	uint8_t connect;
}BG96_InfoTypeDef;

typedef struct
{
	uint8_t status;
	uint8_t data[MAX_BUFFER_SIZE];
	uint32_t lenth;
	uint8_t *headpos;
	uint8_t *tailpos;
}BG96_SocketTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#define BG96_SERVER_IP		"106.14.32.12"		// "115.28.232.174"
#define BG96_SERVER_PORT	5000							// 2317
#define BG96_NTP_IP				"202.120.2.101"
#define BG96_NTP_PORT			123

/* Exported macro ------------------------------------------------------------*/
extern BG96_InfoTypeDef		BG96_Info;
extern BG96_SocketTypeDef	BG96_Socket[1];

/* Exported functions ------------------------------------------------------- */
extern BG96_StatusTypeDef BG96_ReadData(int socket, uint8_t *buff, uint32_t read_len, uint32_t *recv_len);
extern BG96_StatusTypeDef BG96_NTPSynchronize(uint8_t contextID, uint8_t *remote_addr, uint32_t remote_port, uint8_t* time);
extern BG96_StatusTypeDef BG96_QueryRTC(uint8_t *time);
extern BG96_StatusTypeDef BG96_Open(uint8_t contextID, int socket, uint8_t *type, uint8_t *remote_addr, uint32_t remote_port);
extern BG96_StatusTypeDef BG96_STATE(uint8_t contextID, int socket);
extern BG96_StatusTypeDef BG96_Send(int socket, uint8_t *data, uint32_t len);
extern BG96_StatusTypeDef BG96_SendEX(int socket, uint8_t *data, uint32_t len);
extern BG96_StatusTypeDef BG96_RetrieveData(int socket, uint8_t *data, uint32_t read_len, uint32_t* recv_len);
extern BG96_StatusTypeDef BG96_Close(int socket);
extern BG96_StatusTypeDef BG96_Init(void);
extern void BG96_Thread(void const *argument);

#endif
