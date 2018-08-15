/**
  ******************************************************************************
  * @file    app_mqtt.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-November-2015
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "app_mqtt.h"
#include "MQTTPacket.h"

#include "bsp_flash.h"
#include "bsp_rtc.h"
#include "app_bg96.h"
#include "app_bg96_io.h"
#include "cJSON.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	int qos;
	unsigned char dup;
	unsigned char retained;
	unsigned short msgid;
	void *payload;
	size_t payloadlen;
	MQTTString topic;
}MQTTConf_TypeDef;

#define MQTT_BUFFER_LEN		512

typedef struct {
	int socket;
	int isconnected;
	unsigned int  open_retry_cnt;
	unsigned int  send_retry_cnt;
	unsigned int  heartbeat_retry_cnt;				// heartbeat retry cnt
	unsigned int 	ping_timer;	
	unsigned char buffer[MQTT_BUFFER_LEN];
	unsigned int  buffer_len;
	unsigned char recv_buffer[MQTT_BUFFER_LEN];
	unsigned int  recv_head;
	unsigned int  recv_tail;
}MQTTClient_TypeDef;

typedef enum
{ 
	MQTT_OK											= 0,
	MQTT_ERROR									= 1,
	MQTT_ALREADY_CONNECTED			= 2,
	MQTT_CONNECTION_CLOSED			= 3,
	MQTT_TIMEOUT								= 4,
} MQTT_StatusTypeDef;

/* Private define ------------------------------------------------------------*/
#define MQTT_CLIENT_ID          "1007000001"
#define MQTT_CLIENT_USERNAME    "zhoujishan"
#define MQTT_CLIENT_PASSWORD    "welcome1"
#define MQTT_CLIENT_KEEPALIVE   6000			// 60s
#define MQTT_CLIENT_SUB_TOPIC   "1007000001/SDSC/GET"
#define MQTT_CLIENT_PUB_TOPIC   "1007000001/SDSC/PUT"
#define MQTT_CLIENT_CONF_TOPIC  "1007000001/SDSC/CONF"
#define MQTT_CMD_TIMEOUT        5000			// 5s
#define MQTT_KEEPALIVE_TMIE			10000			// 10s
//#define MQTT_CLIENT_PUB_MSG     "mqtt_client_test_data_1234567890"
#define MQTT_SERVER             "139.196.141.220"//"139.224.9.135"
#define MQTT_SERVER_PORT        1883

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
MQTTClient_TypeDef  MQTT_data;
MQTTConf_TypeDef	MQTT_conf;
uint32_t mqtt_upload_count;
uint8_t *msg_send = NULL;
int msg_send_len;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int MQTT_GetData(uint8_t *buffer, int len)
{
	int read_len = 0;

	if(len <= MQTT_data.recv_tail)
	{
		read_len = len;
	}
	else
	{
		read_len = MQTT_data.recv_tail;
	}
	memcpy(buffer, &MQTT_data.recv_buffer[MQTT_data.recv_head], read_len);
	MQTT_data.recv_head += read_len;
	
	return read_len;
}

void pub_packet_init(void)
{
	SRAM_Malloc_Init(SRAMIN);
	memcpy(dev_info.device_id, MQTT_CLIENT_ID, strlen(MQTT_CLIENT_ID));
}

MQTT_StatusTypeDef MQTT_Network_Open(int socket, uint8_t* addr, uint32_t port)
{
	int ret;
	ret = BG96_Open(1, socket, "TCP", addr, port);
	if(ret == BG96_ALREADY_CONNECTED)
		return MQTT_ALREADY_CONNECTED;
	else if(ret == BG96_ERROR)
		return MQTT_ERROR;
	return MQTT_OK;
}

MQTT_StatusTypeDef MQTT_Network_Check(int socket)
{
	int ret;
	ret = BG96_STATE(1, socket);
	if(ret == BG96_OK)
		return MQTT_OK;
	return MQTT_ERROR;
}

MQTT_StatusTypeDef MQTT_Network_Close(int socket)
{
	int ret;
	ret = BG96_Close(socket);
	if(ret == BG96_OK)
		return MQTT_OK;
	return MQTT_ERROR;
}

MQTT_StatusTypeDef MQTT_Connect(MQTTPacket_connectData *data)
{
	int ret;
	uint32_t tickStart = 0;
	
	/* Reset the reception buffer */ 
	memset(MQTT_data.buffer, 0, MQTT_BUFFER_LEN);
	MQTT_data.buffer_len = MQTTSerialize_connect(MQTT_data.buffer, MQTT_BUFFER_LEN, data);
	ret = BG96_Send(MQTT_data.socket, MQTT_data.buffer, MQTT_data.buffer_len);
	
	if(ret != BG96_OK)
		return MQTT_ERROR;
	
	/* Loop until connack received */
	tickStart = HAL_GetTick();

	while((HAL_GetTick() - tickStart ) < MQTT_CMD_TIMEOUT)
	{
		ret = BG96_ReadData(MQTT_data.socket, MQTT_data.recv_buffer, MQTT_BUFFER_LEN, &MQTT_data.recv_tail);
		if(ret == BG96_OK)
		{
			MQTT_data.recv_head = 0;
			break;
		}
		osDelay(10);
	}
	
	/* Reset the reception buffer */ 
	memset(MQTT_data.buffer, 0, MQTT_BUFFER_LEN);
	ret = MQTTPacket_read(MQTT_data.buffer, MQTT_BUFFER_LEN, MQTT_GetData);
	if(ret == CONNACK)
	{
		unsigned char sessionPresent, connack_rc;

		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, MQTT_data.buffer, MQTT_BUFFER_LEN) != 1 || connack_rc != 0)
		{
//			printf("Unable to connect, return code %d\n", connack_rc);
		}
	}
	else
	{
//		printf("ERROR: MQTT ACK ID =%d\r\n", ret);
		return MQTT_ERROR;
	}
	return MQTT_OK;
}

MQTT_StatusTypeDef MQTT_Subscribe(MQTTConf_TypeDef *conf)
{
	int ret;
	uint32_t tickStart = 0;
	
	/* Reset the reception buffer */ 
	memset(MQTT_data.buffer, 0, MQTT_BUFFER_LEN);
	MQTT_data.buffer_len = MQTTSerialize_subscribe(MQTT_data.buffer, MQTT_BUFFER_LEN,
												   MQTT_conf.dup, MQTT_conf.msgid, 1,
												   &MQTT_conf.topic, &MQTT_conf.qos);
	ret = BG96_Send(MQTT_data.socket, MQTT_data.buffer, MQTT_data.buffer_len);
	
	if(ret != BG96_OK)
		return MQTT_ERROR;
	
	/* Loop until connack received */
	tickStart = HAL_GetTick();

	while((HAL_GetTick() - tickStart ) < MQTT_CMD_TIMEOUT)
	{
		ret = BG96_ReadData(MQTT_data.socket, MQTT_data.recv_buffer, MQTT_BUFFER_LEN, &MQTT_data.recv_tail);
		if(ret == BG96_OK)
		{
			MQTT_data.recv_head = 0;
			break;
		}
		osDelay(10);
	}
	
	/* Reset the reception buffer */ 
	memset(MQTT_data.buffer, 0, MQTT_BUFFER_LEN);
	ret = MQTTPacket_read(MQTT_data.buffer, MQTT_BUFFER_LEN, MQTT_GetData);
	if(ret == SUBACK)
	{
		unsigned short submsgid;
		int subcount;
		int granted_qos;

		ret = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, MQTT_data.buffer, MQTT_BUFFER_LEN);
		if (granted_qos != 0)
		{
//			printf("granted qos != 0, %d\n", granted_qos);
		}
	}
	else
	{
//		printf("ERROR: MQTT ACK ID =%d\r\n", ret);
		return MQTT_ERROR;
	}
	return MQTT_OK;
}

MQTT_StatusTypeDef MQTT_MessagePing(void)
{
	int ret;
	
	memset(MQTT_data.buffer, 0, MQTT_BUFFER_LEN);
	MQTT_data.buffer_len = MQTTSerialize_pingreq(MQTT_data.buffer, MQTT_BUFFER_LEN);
	ret = BG96_Send(MQTT_data.socket, MQTT_data.buffer, MQTT_data.buffer_len);

	if(ret == BG96_ERROR)
		return MQTT_ERROR;
	return MQTT_OK;
}

void JSON_GeneratePacket(void)
{
	cJSON *pnod=NULL, *pnod_values=NULL;
	
	pnod = cJSON_CreateObject();
	pnod_values = cJSON_CreateObject();

	cJSON_AddItemToObject(pnod, "deviceId", cJSON_CreateString((char *)dev_info.device_id));
	BSP_RTC_TimeStampRead(dev_info.timestamp);
	cJSON_AddItemToObject(pnod, "timestamp", cJSON_CreateString((char *)dev_info.timestamp));
	cJSON_AddItemToObject(pnod, "values", pnod_values);

	cJSON_AddItemToObject(pnod_values, "MF00", cJSON_CreateNumber(dev_info.MF00));
	cJSON_AddItemToObject(pnod_values, "MF01", cJSON_CreateNumber(dev_info.MF01));
	
	msg_send = (uint8_t *)cJSON_Print(pnod);
	msg_send_len = strlen((char*)msg_send);
	cJSON_Delete(pnod_values);
	cJSON_Delete(pnod);
}

unsigned char JSON_ParsingPacket(uint8_t topic, uint8_t *data)
{
	cJSON *pnod=NULL, *pnod_values=NULL, *psub=NULL;

	pnod = cJSON_Parse((char *)data);
	cJSON_free(data);
	if(pnod == NULL)
	{
		return NULL;
	}

//	psub = cJSON_GetObjectItem(pnod, "timestamp");
//	printf("timestamp: %s\r\n", psub->valuestring);
//	memcpy(dev_info.timestamp, psub->valuestring, strlen(psub->valuestring));

	pnod_values = cJSON_GetObjectItem(pnod, "values");
	
	if(topic == 0)
	{
		psub = cJSON_GetObjectItem(pnod_values, "MF00");
//		printf("MF00: %s\r\n", psub->valuestring);
		dev_info.MF00 = atoi(psub->valuestring);
		psub = cJSON_GetObjectItem(pnod_values, "MF00");
//		printf("MF00: %s\r\n", psub->valuestring);
		dev_info.MF00 = atoi(psub->valuestring);
	}
	else if(topic == 1)
	{
		psub = cJSON_GetObjectItem(pnod_values, "UploadPeriod");
//		printf("UploadPeriod: %s\r\n", psub->valuestring);
		dev_conf.UploadPeriod = atoi(psub->valuestring);
		psub = cJSON_GetObjectItem(pnod_values, "SamplingPeriod");
//		printf("SamplingPeriod: %s\r\n", psub->valuestring);
		dev_conf.SamplingPeriod = atoi(psub->valuestring);
		BSP_Flash_WriteWord(FLASH_START_ADDR, (uint32_t *)&dev_conf, sizeof(dev_conf));
	}
	
	cJSON_Delete(psub);
	cJSON_Delete(pnod_values);
	cJSON_Delete(pnod);

	return 1;
}

MQTT_StatusTypeDef MQTT_MessagePublish(void)
{
	MQTTString SendTopic;
//	uint8_t *msg_send = NULL;
//	int msg_send_len;
	int rc;
	
	/* loop getting msgs on subscribed topic */
	SendTopic.cstring = MQTT_CLIENT_PUB_TOPIC;
	JSON_GeneratePacket();
	
	memset(MQTT_data.buffer, 0, MQTT_BUFFER_LEN);
	MQTT_data.buffer_len = MQTTSerialize_publish(MQTT_data.buffer, MQTT_BUFFER_LEN, 0, 0, 0, 0, SendTopic, (uint8_t *)msg_send, msg_send_len);
	rc = BG96_Send(MQTT_data.socket, MQTT_data.buffer, MQTT_data.buffer_len);
	cJSON_free(msg_send);
	if(rc != 0)
		return MQTT_ERROR;
	
	return MQTT_OK;
}

MQTT_StatusTypeDef MQTT_PingReq(void)
{
	int rc;

	memset(MQTT_data.buffer, 0, MQTT_BUFFER_LEN);
	MQTT_data.buffer_len = MQTTSerialize_pingreq(MQTT_data.buffer, MQTT_BUFFER_LEN);
	rc = BG96_Send(MQTT_data.socket, MQTT_data.buffer, MQTT_data.buffer_len);

	if(rc != 0)
		return MQTT_ERROR;
	
	return MQTT_OK;
}

MQTT_StatusTypeDef MQTT_MessageArrive(void)
{
	MQTTConf_TypeDef MQTTConf_Recv;
	int ret;
	uint8_t *msg_recv;
	int msg_recv_len;
	
	ret = MQTTDeserialize_publish(&MQTTConf_Recv.dup, &MQTTConf_Recv.qos,
								 &MQTTConf_Recv.retained, &MQTTConf_Recv.msgid,
								 &MQTTConf_Recv.topic, &msg_recv, &msg_recv_len,
								 MQTT_data.buffer, MQTT_BUFFER_LEN);
	if(ret == 1)
	{
		if( 0 == strcmp(MQTTConf_Recv.topic.cstring , MQTT_CLIENT_PUB_TOPIC) )
		{
			JSON_ParsingPacket(0, msg_recv);
//			printf("message arrived %.*s\n", msg_recv_len, msg_recv);
		}
		else if( 0 == strcmp(MQTTConf_Recv.topic.cstring , MQTT_CLIENT_CONF_TOPIC) )
		{
			JSON_ParsingPacket(1, msg_recv);
//			printf("message arrived from config topic\r\n");
		}
	}
	return MQTT_OK;
}

MQTT_StatusTypeDef MQTT_PraseACK(void)
{
	int ret;
	
	ret = BG96_ReadData(0, MQTT_data.recv_buffer, MQTT_BUFFER_LEN, &MQTT_data.recv_tail);
	if(ret != BG96_OK)
		return MQTT_ERROR;
	
	MQTT_data.recv_head = 0;
	memset(MQTT_data.buffer, 0, MQTT_BUFFER_LEN);
	ret = MQTTPacket_read(MQTT_data.buffer, MQTT_BUFFER_LEN, MQTT_GetData);
	switch(ret)
	{
//		case CONNACK: printf("mqtt connect ok. type:%d\r\n", ret); break;
//		case PUBLISH: printf("mqtt publish. type:%d\r\n", ret);
//			MQTT_MessageArrive(); break;
//		case PUBACK: printf("mqtt puback. type:%d\r\n", ret); break;
//		case PUBREC: printf("mqtt pubrcv. type:%d\r\n", ret); break;
//		case PUBREL: printf("mqtt pubrel. type:%d\r\n", ret); break;
//		case PUBCOMP: printf("mqtt pubcomp. type:%d\r\n", ret); break;
//		case SUBACK: printf("mqtt suback. type:%d\r\n", ret); break;
//		case UNSUBACK: printf("mqtt unsuback. type:%d\r\n", ret); break;
//		case PINGREQ: printf("mqtt ping rcv. type:%d\r\n", ret); break;
//		case PINGRESP: printf("mqtt pingresp. type:%d\r\n", ret); break;
//		case DISCONNECT: printf("mqtt disconnect\r\n"); return MQTT_CONNECTION_CLOSED;
		
		case CONNACK: break;
		case PUBLISH: MQTT_MessageArrive(); break;
		case PUBACK: break;
		case PUBREC: break;
		case PUBREL: break;
		case PUBCOMP: break;
		case SUBACK:  break;
		case UNSUBACK: break;
		case PINGREQ: break;
		case PINGRESP: break;
		case DISCONNECT: return MQTT_CONNECTION_CLOSED;
		
		default :break;
	}
	return MQTT_OK;
}

void MQTT_Thread(void const *argument)
{
	MQTTPacket_connectData client = MQTTPacket_connectData_initializer;
	MQTT_StatusTypeDef ret;
	
	
MQTT_start:	
	BG96_IO_Init();
	if(BG96_Init() != BG96_OK)
	{
		while(BG96_Init() != BG96_OK)
		{                                                                               
			osDelay(5000);
		}
	}
	
	if(BG96_OK != BG96_NTPSynchronize(1, BG96_NTP_IP, BG96_NTP_PORT, BG96_Info.time))
	{
		BG96_QueryRTC(BG96_Info.time);
	}
	pub_packet_init();
	MQTT_data.isconnected = false;
	MQTT_data.socket = 0;
	MQTT_data.recv_head = 0;
	MQTT_data.recv_tail = 0;
	MQTT_conf.qos = 0;
	MQTT_conf.msgid =1;

// 1. create network connection
network_reconnect:
	ret = MQTT_Network_Open(MQTT_data.socket, MQTT_SERVER, MQTT_SERVER_PORT);
	if(ret == MQTT_ERROR)
	{
		osDelay(5000);
		if(MQTT_data.open_retry_cnt >= 5)
		{
			MQTT_data.open_retry_cnt = 0;
			BG96_IO_RST(10000);
			goto MQTT_start;
		}
		MQTT_data.open_retry_cnt++;
		goto network_reconnect;
	}
	else if(ret == MQTT_ALREADY_CONNECTED)
	{
//		printf("MQTT Sock Already open...\r\n");
	}
//	printf("mqtt sock_id:%d\r\n", MQTT_data.socket);
// 2. init mqtt client
	
/* 3. mqtt client creat connection*/
	client.cleansession = 1;
	client.clientID.cstring = MQTT_CLIENT_ID;
	client.username.cstring = MQTT_CLIENT_USERNAME;
	client.password.cstring = MQTT_CLIENT_PASSWORD;
	client.keepAliveInterval = MQTT_CLIENT_KEEPALIVE;
	
	ret = MQTT_Connect(&client);
	if(ret != MQTT_OK)
	{
		osDelay(1000);
//		goto MQTT_disconnect;
	}

/* 4. mqtt client subscribe topic*/
	MQTT_conf.topic.cstring = MQTT_CLIENT_SUB_TOPIC;
	ret = MQTT_Subscribe(&MQTT_conf);
	if(ret != MQTT_OK)
	{
		osDelay(1000);
//		goto MQTT_disconnect;
	}
	
	MQTT_data.isconnected = true;
	
/* 5. client loop for recv msg && keepalive*/
	while(1)
	{
		MQTT_PraseACK();

		if(mqtt_upload_count >= dev_conf.UploadPeriod)
		{
			mqtt_upload_count = 0;
			ret = MQTT_MessagePublish();
			if (ret == MQTT_OK)
			{
				MQTT_data.send_retry_cnt = 0;
//				printf("Successfully published\r\n");
			}
			else
			{
				MQTT_data.send_retry_cnt++;
//				printf("Publish failed\r\n");
			}
			if(MQTT_data.send_retry_cnt >= 5)
			{
				MQTT_data.send_retry_cnt = 0;
				goto MQTT_disconnect;
			}
		}
		osDelay(10);
		mqtt_upload_count++;
	}

MQTT_disconnect:
//	printf("MQTT client disconnected, reconnect after 5s...\r\n");
	osDelay(5000);
	MQTT_Network_Close(MQTT_data.socket);
	goto network_reconnect;
}