/**
  ******************************************************************************
  * @file    app_BG96.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bg96.h"

#include "bsp_rtc.h"

#include "bg96_io.h"
#include "dev_utils.h"


/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t BG96_ATCmdBuffer[MAX_AT_CMD_SIZE];
uint8_t BG96_Buffer[MAX_BUFFER_SIZE];
BG96_InfoTypeDef	BG96_Info = {0};

/* Private function prototypes -----------------------------------------------*/
static BG96_StatusTypeDef BG96_SendATCmd(uint8_t* cmd, uint32_t Length, const uint8_t* Token);
//static BG96_StatusTypeDef BG96_RecvATCmd(uint8_t* Buffer, uint32_t Length, uint32_t* RetLength);
static BG96_StatusTypeDef BG96_QueryCPIN(void);
static BG96_StatusTypeDef BG96_QueryCIMI(void);
static BG96_StatusTypeDef BG96_QueryCSQ(void);
static BG96_StatusTypeDef BG96_QueryCGATT(void);
static BG96_StatusTypeDef BG96_QueryCEREG(void);

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Run the AT command
  * @param  cmd the buffer to fill will the received data.
  * @param  Length the maximum data size to receive.
  * @param  Token the expected output if command runs successfully
  * @retval returns BG96_OK on success and BG96_ERROR otherwise.
  */
BG96_StatusTypeDef BG96_SendATCmd(uint8_t* cmd, uint32_t Length, const uint8_t* Token)
{
	uint32_t tickStart = 0;
	uint32_t read_len = 0;
	/* Reset the Rx buffer to make sure no previous data exist */
	memset(BG96_Buffer, '\0', MAX_BUFFER_SIZE);
  
	/* Send the command */
	if (BG96_IO_Send(cmd, Length) < 0)
	{
		return BG96_ERROR;
	}

	/* Loop until data received */
	tickStart = HAL_GetTick();
	
	while((HAL_GetTick() - tickStart ) < AT_CMD_TIMEOUT)
	{
		/* Wait for reception */
		if( (read_len = BG96_IO_Receive(BG96_Buffer, MAX_BUFFER_SIZE)) > 0 )
		{
			/* Extract the Token */
			if (memstr((char *)BG96_Buffer, read_len, (char *)Token) != NULL)
			{
				return BG96_OK;
			}
			
			/* Check if the message contains error code */
			else if (strstr((char *)BG96_Buffer, AT_ERROR_STRING) != NULL)
			{
				return BG96_ERROR;
			}
		}
		osDelay(5);
	}
	return BG96_TIMEOUT;
}

/**
  * @brief  receive data over the wifi connection.
  * @param  pData: the buffer to fill will the received data.
  * @param  Length: the maximum data size to receive.
  * @param  RetLength: the actual data received.
  * @retval returns BG96_OK on success and BG96_ERROR otherwise.
  */
BG96_StatusTypeDef BG96_ReadData(int socket, uint8_t *buff, uint32_t read_len, uint32_t *recv_len)
{
	BG96_StatusTypeDef ret;
	char *token;
	int socket_id;

	/* Reset the reception buffer */  
	memset(BG96_Buffer, '\0', MAX_BUFFER_SIZE);

	/* When reading data over a wifi connection the bg96
	 splits it into chunks of 1460 bytes maximum each, each chunk is preceded
	 by the string "+IPD,<chunk_size>:". Thus to get the actual data we need to:
	   - Receive data until getting the "+IPD," token, a new chunk is marked.
	   - Extract the 'chunk_size' then read the next 'chunk_size' bytes as actual data
	   - Mark end of the chunk.
	   - Repeat steps above until no more data is available. */

	if (BG96_IO_Receive(BG96_Buffer, MAX_BUFFER_SIZE) != 0)
	{
		/* When a new chunk is met, extact its size */
		if( (token = strstr((char *)BG96_Buffer, AT_RECV_STRING)) != NULL )
		{
			token += 6;			// skip recv", to get socket id
			socket_id = *token-0x30;
			ret = BG96_RetrieveData(socket_id, buff, read_len, recv_len);
			if( ret != BG96_OK)
			{
				return BG96_ERROR;
			}
		}

		/* Check if message contains error code */
		else if (strstr((char *)BG96_Buffer, AT_ERROR_STRING) != NULL)
		{
			return BG96_ERROR;
		}
	}
	else
	{
		return BG96_ERROR;
	}
	
	return BG96_OK;
}

/******************************************************************************/
/*                 				BG96 AT Commond				                  */
/*  Add here the AT Commond Handler 										  */
/******************************************************************************/

/**
  * Brief   Query.
  * Param   None.
  * retval  AT Status.
  */
BG96_StatusTypeDef BG96_QueryCPIN(void)
{
	BG96_StatusTypeDef ret;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+CPIN?%c%c", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	return ret;
}

/**
  * Brief   Query.
  * Param   None.
  * retval  AT Status.
  */
BG96_StatusTypeDef BG96_QueryCIMI(void)
{
	BG96_StatusTypeDef ret;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+CIMI%c%c", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	return ret;
}

/**
  * Brief   Query.
  * Param   rssi: .
  * Param   ber: .
  * retval  AT Status.
  */
BG96_StatusTypeDef BG96_QueryCSQ(void)
{
	BG96_StatusTypeDef ret = BG96_OK;
	char *token, *temp;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+CSQ%c%c", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	
	if(ret == BG96_OK)
	{
		token = strstr((char*)BG96_Buffer, "+CSQ:");
		token += 5;
		temp = strstr(token, "\r\n");
		*temp = '\0';
 
    	/* Get the CSQ value */
    	strcpy((char*)BG96_Info.csq, token);
	}
	return ret;
}

/**
  * Brief   Query.
  * Param   None.
  * retval  AT Status.
  */
BG96_StatusTypeDef BG96_QueryCGATT(void)
{
	BG96_StatusTypeDef ret = BG96_OK;
	char *token;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+CGATT?%c%c", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	
	if(ret == BG96_OK)
	{
		token = strstr((char*)BG96_Buffer, "+CGATT:");// "+CGATT: 1\r\n"
		token += 8;

    	/* Get the CSQ value */
    	BG96_Info.cgatt = *token;
	}
	return ret;
}

/**
  * Brief   Query.
  * Param   None.
  * retval  AT Status.
  */
BG96_StatusTypeDef BG96_QueryCEREG(void)
{
	BG96_StatusTypeDef ret = BG96_OK;
	char *token;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+CEREG?%c%c", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	
	if(ret == BG96_OK)
	{
		token = strstr((char*)BG96_Buffer, "+CEREG:");	// "+CEREG: 0,1\r\n"
		token += 10;

		/* Get the CSQ value */
		BG96_Info.cereg = *token;
	}
	return ret;
}

/**
  * Brief   Synchronize Local Time with NTP Server.
  * Param   contextID: None.
  * Param   remote_addr: NTP Server address.
  * Param   remote_port: NTP Server port.
  * Param   time: Pointer to time buffer.
  * retval  AT Status.
  */
BG96_StatusTypeDef BG96_NTPSynchronize(uint8_t contextID, uint8_t *remote_addr, uint32_t remote_port, uint8_t *time)
{
	BG96_StatusTypeDef ret = BG96_OK;
	char *token, *temp;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+QNTP=%d,\"%s\",%d,%d%c%c", contextID, remote_addr, remote_port, 1, '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_NTP_UPDATE_STRING);

	if( ret == BG96_OK )
	{
		token = strstr((char*)BG96_Buffer, "+QNTP:");
		token += 12;
		temp = strstr(token, "\"");		// +QNTP: 0,"2017/08/17,03:29:11"
		memcpy(time, token, temp-token);
		BSP_RTC_TimeStampWrite(time);
	}
	return ret;
}

/**
  * Brief   Synchronize Local Time with NTP Server.
  * Param   contextID: None.
  * Param   remote_addr: NTP Server address.
  * Param   remote_port: NTP Server port.
  * Param   time: Pointer to time buffer.
  * retval  AT Status.
  */
BG96_StatusTypeDef BG96_QueryRTC(uint8_t *time)
{
	BG96_StatusTypeDef ret = BG96_OK;
	char *token, *temp;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+CCLK?%c%c", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);

	if( ret == BG96_OK )
	{
		token = strstr((char*)BG96_Buffer, "+CCLK:");
		token += 8;
		temp = strstr(token, "\"");		// +CCLK: "18/02/05,03:29:11"
		memcpy(time, token, temp-token);
		BSP_RTC_TimeStampWrite(time);
	}
	return ret;
}

/**
  * Brief   Open a Socket Service.
  * Param   contextID: None.
  * Param   socket: None.
  * Param   type: None.
  * Param   remote_addr: None.
  * Param   remote_port: None.
  * retval  AT Status.
  */
BG96_StatusTypeDef BG96_Open(uint8_t contextID, int socket, uint8_t *type, uint8_t *remote_addr, uint32_t remote_port)
{
	BG96_StatusTypeDef ret = BG96_OK;
	char* token;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char*)BG96_ATCmdBuffer, "AT+QIOPEN=%d,%d,\"%s\",\"%s\",%d,0,0%c%c", contextID, socket, type, remote_addr, remote_port, '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), "+QIOPEN:");
	
	if(ret == BG96_OK)
	{
		token = strstr((char*)BG96_Buffer, "+QIOPEN:");
		token += 11;			// skip "+QIOPEN:"

    	if( (*token-0x30) == socket )
		{
//			BG96_Sock_Init(socket);
		}
		else if( *token == '5')
		{
			ret = BG96_ALREADY_CONNECTED;
		}
	}
	return ret;
}

/**
  * Brief   Query Socket Service Status.
  * Param   contextID: None.
  * Param   socket: None.
  * retval  AT Status.
  */
BG96_StatusTypeDef BG96_STATE(uint8_t contextID, int socket)
{
	BG96_StatusTypeDef ret = BG96_OK;
	char* token;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+QISTATE=%d,%d%c%c", contextID, socket, '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	
	if(ret == BG96_OK)
	{
		token = strstr((char*)BG96_Buffer, "+QISTATE:");
		if(token == 0)
		{
//			ret = BG96_ALREADY_CONNECTED;
			return BG96_ERROR;
		}
	}
	return ret;
}

/**
  * Brief   Send Data.
  * Param   socket: socketid.
  * Param   data: Pointer to data buffer.
  * Param   len: Amount of data to be sent.
  * retval  AT status.
  */
BG96_StatusTypeDef BG96_Send(int socket, uint8_t* data, uint32_t len)
{
	BG96_StatusTypeDef ret = BG96_OK;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+QISEND=%d,%d%c%c", socket, len, '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_SEND_PROMPT_STRING);
	
	if(ret != BG96_OK)
	{
		return BG96_ERROR;
	}
	/* Wait before sending data. */
	osDelay(100);
	
	/* Send the data */
	ret = BG96_SendATCmd(data, len, (uint8_t*)AT_SEND_OK_STRING);
	return ret;
}

/**
  * Brief   Send Hex String.
  * Param   socket: socketid
  * Param   data: Pointer to data buffer.
  * Param   len: Amount of data to be sent.
  * retval  AT status
  */
BG96_StatusTypeDef BG96_SendEX(int socket, uint8_t *data, uint32_t len)
{
	uint32_t i;
	char tmp_buf[5];
	BG96_StatusTypeDef ret;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+QISEND=%d,\"", socket);
	for(i = 0; i < len; i++)
	{
	    sprintf(tmp_buf, "%02X", data[i]);
	    strcat((char*)BG96_ATCmdBuffer, tmp_buf);
	}
	strcat((char*)BG96_ATCmdBuffer, "\"\r\n");
	
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_SEND_OK_STRING);
	return ret;
}

/**
  * Brief   retrieve the Received TCP/IP Data.
  * Param   socket: socketid
  * Param   read_len: Amount of data to read.
  * Param   data: Pointer to data buffer
  * retval  recv_len
  */
BG96_StatusTypeDef BG96_RetrieveData(int socket, uint8_t *data, uint32_t read_len, uint32_t* recv_len)
{
	BG96_StatusTypeDef ret;
	char LengthString[6];
	char *token, *temp;
	
	/* Construct the command */
	memset(BG96_ATCmdBuffer, 0, MAX_AT_CMD_SIZE);
	sprintf((char*)BG96_ATCmdBuffer, "AT+QIRD=%d,%d%c%c", socket, read_len, '\r', '\n'); 
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	
	if(ret != BG96_OK )
	{
		return BG96_ERROR;
	}
	
	token = strstr((char *)BG96_Buffer, "+QIRD: ");	// +QIRD: 4
	token += 7;
	temp = strstr(token, "\r\n");
	// find "len\r\n" and save
	memcpy(LengthString, token, temp-token);
	LengthString[temp - token] = '\0';
	*recv_len = atoi(LengthString);
//	sscanf(tmp_buf, "%d", recv_len);
	token = temp + 2;						// skip "\r\n"
	memcpy(data, token, *recv_len);

	return ret;
}

/**
  * Brief   Close a Socket Service.
  * Param   socketid
  * retval  AT status
  */
BG96_StatusTypeDef BG96_Close(int socket)
{
	BG96_StatusTypeDef ret;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char*)BG96_ATCmdBuffer, "AT+QICLOSE=%d%d%d", socket, '\r', '\n');
	
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	return ret;
}

/**
  * Brief   Close a Socket Service.
  * Param   socketid
  * retval  AT status
  */
BG96_StatusTypeDef BG96_Sleep(void)
{
	BG96_StatusTypeDef ret;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char*)BG96_ATCmdBuffer, "AT+QSCLK=1%d%d", '\r', '\n');
	
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	BG96_IO_Sleep();
	return ret;
}

/**
  * Brief   Close a Socket Service.
  * Param   socketid
  * retval  AT status
  */
BG96_StatusTypeDef BG96_Wakeup(void)
{
	BG96_StatusTypeDef ret;
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char*)BG96_ATCmdBuffer, "AT+QPWOD=%d%d", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	BG96_IO_Wakeup();
	return ret;
}


/**
  * @brief   Initialize the bg96 module.
  *          IT intitalize the IO to communicate between the MCU and the module, then
  *          test that the modules is working using some basic AT commands.
  *          in case of success the string "OK" is returned inside otherwise
  *	         it is an error.
  * @param   None
  * @retval  BG96_OK on sucess, BG96_ERROR otherwise.
  */
BG96_StatusTypeDef BG96_Init(void)
{
	BG96_StatusTypeDef ret;
  
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT%c%c", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	/* Exit in case of error */
	if (ret !=  BG96_OK)
	{
		printf("BG96 AT ERR...Send Again afte 1s...\r\n");
		osDelay(1000);
		ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
		if (ret !=  BG96_OK)
			return BG96_ERROR;
	}

	/* Disable the Echo mode */
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "ATE0%c%c", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	/* Exit in case of error */
	if (ret !=  BG96_OK)
		return BG96_ERROR;
	
	if(BG96_QueryCPIN() != BG96_OK)		// Query SIM card status
		return BG96_ERROR;

	if(BG96_QueryCIMI() != BG96_OK)		// Query the IMSI
		return BG96_ERROR;

	while(BG96_Info.cereg != '1')
	{
		if(BG96_QueryCEREG() != BG96_OK)
			osDelay(500);
		if(BG96_Info.cereg == '4')
		{
			memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
			sprintf((char *)BG96_ATCmdBuffer, "AT+QCFG=\"iotopmode\",1,1%c%c", '\r', '\n');
			/* Send the command */
			ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
			/* Exit in case of error */
			if (ret !=  BG96_OK)
				return BG96_ERROR;
		}
	}
	
	if(BG96_QueryCSQ() != BG96_OK)		// Query signal strength
		return BG96_ERROR;
	
	if(BG96_QueryCGATT() != BG96_OK)	// Query signal strength
		return BG96_ERROR;
	
	/* Configure Parameters of a TCP/IP Context. */
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+QICSGP=1%c%c", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), "+QICSGP");
	/* Exit in case of error */
	if (ret !=  BG96_OK)
		return BG96_ERROR;
  
	/* Activate a PDP Context. */
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+QIACT=1%c%c", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), (uint8_t*)AT_OK_STRING);
	/* Exit in case of error */
	if (ret !=  BG96_OK)
		return BG96_ERROR;

	/* Query a PDP Context. */
	
	/* Construct the command */
	memset (BG96_ATCmdBuffer, '\0', MAX_AT_CMD_SIZE);
	sprintf((char *)BG96_ATCmdBuffer, "AT+QIACT?%c%c", '\r', '\n');
	/* Send the command */
	ret = BG96_SendATCmd(BG96_ATCmdBuffer, strlen((char *)BG96_ATCmdBuffer), "+QIACT");
    /* Exit in case of error */
	if (ret !=  BG96_OK)
		return ret;
//	printf("BG96 Init OK\r\n");
	
	return ret;
}

///**
//  * @brief  BG96 Thread
//  * @param  thread not used
//  * @retval None
//  */
//void BG96_Thread(void const *argument)
//{
//	uint32_t cnt=0, recv_cnt;
//	uint8_t buff[1024] = {0};
//	
//	/* Configuration the IO low layer */
//	BG96_IO_Init();
//
//	if(BG96_Init() != BG96_OK)
//	{
//		while(BG96_Init() != BG96_OK)
//		{
//			printf("BG96 Init Error, Resart After 5s...\r\n");
//			osDelay(2000);
//		}
//	}
//	
//	if(BG96_OK == BG96_NTPSynchronize(1, BG96_NTP_IP, BG96_NTP_PORT, BG96_Info.time))
//	{
//		printf("BG96 Update time OK\r\n");
//	}
//	else
//		printf("BG96 Update Time Failed\r\n");
//
//	while(1)
//	{
//		LL_mDelay(2000);
//		BG96_QueryRTC(BG96_Info.time);
//	}
//	
//	BG96_Info.connect = 1;
////	if ( BG96_OK == BG96_Open(1, 0, "TCP", BG96_SERVER_IP, BG96_SERVER_PORT) )
////	{
////		printf("BG96 Socket Creat OK\r\n");
////	}
//	if ( BG96_OK == BG96_Open(1, 0, "UDP", BG96_SERVER_IP, BG96_SERVER_PORT) )
//	{
//		printf("BG96 Socket Creat OK\r\n");
//	}
////	BG96_Sleep();
//	
//	for (;;)
//	{
//		osDelay(50);
//		cnt++;
//		if( cnt >= 100)
//		{
////			BG96_Wakeup();
//			cnt = 0;
//			BG96_Send(0, "helloWorld!\r\n", 13);
//		}
//		if( BG96_OK == BG96_ReadData(0, buff, 1024, &recv_cnt) )
//		{
//			printf("recv dat.\r\n");
//		}
//	}
//}
