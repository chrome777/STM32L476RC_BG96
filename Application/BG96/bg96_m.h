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
#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "bg96_conf.h"


/* Private Constants --------------------------------------------------------*/
#define  RET_NONE           0x0000  /* RET_NONE shall be 0x0: don't change this value! */
#define  RET_CRLF           0x0001
#define  RET_OK             0x0002  /* do not change this value */
#define  RET_SIM_READY      0x0004
#define  RET_ARROW          0x0008
#define  RET_SENT           0x0010  /* do not change this value */
#define  RET_OPEN           0x0020
#define  RET_ERROR          0x0040  /* do not change this value */
#define  RET_URC_CLOSED     0x0080
#define  RET_URC_RECV       0x0100
#define  RET_URC_IN_FULL    0x0200
#define  RET_URC_INCOM      0x0400
#define  RET_URC_PDPDEACT   0x0800
#define  RET_URC_DNS        0x1000
#define  RET_BUF_FULL       0x2000  /* do not change this value */
#define  RET_READ           0x4000
#define  RET_CME_ERROR      0x8000  /* do not change this value */
#define  RET_CMS_ERROR      0x10000
#define  RET_POWERED_DOWN   0x20000
#define  RET_SEND           0x40000
#define  NUM_RESPONSES  		18

/* Timeouts modem dependent */
#define BG96_TOUT_SHORT                         50  /* 50 ms */
#define BG96_TOUT_300                          350  /* 0,3 sec + margin */
#define BG96_TOUT_ATSYNC                       500
/* Timeouts network dependent */   
#define BG96_TOUT_5000                        5500  /* 5 sec + margin */
#define BG96_TOUT_15000                      16500  /* 15 sec + margin */
#define BG96_TOUT_40000                      42000  /* 40 sec + margin */
#define BG96_TOUT_60000                      64000  /* 1 min + margin */
#define BG96_TOUT_75000                      78000  /* 75 sec + margin */
#define BG96_TOUT_150000                    156000  /* 2,5 min + margin */
#define BG96_TOUT_180000                    186000  /* 3 min + margin */

#define BG96_MAX_APN_NAME_SIZE                  32
#define BG96_MAX_USER_NAME_SIZE                 32
#define BG96_MAX_PSW_NAME_SIZE                  32

#define BG96_ERROR_STRING_SIZE                  40

#define BG96_MFC_SIZE                           10
#define BG96_PROD_ID_SIZE                        6
#define BG96_FW_REV_SIZE                        16
#define BG96_IMEI_SIZE                          16
#define BG96_ICCID_SIZE                         20
#define BG96_IMSI_SIZE                          16

/* Exported macro ------------------------------------------------------------*/
#define MIN(a, b)  ((a) < (b) ? (a) : (b))

/* Exported types ------------------------------------------------------------*/
typedef int8_t (*IO_Init_Func)( void);
typedef int8_t (*IO_DeInit_Func)( void);
typedef int8_t (*IO_Baudrate_Func)(uint32_t BaudRate);
typedef void (*IO_Flush_Func)(void);
typedef int16_t (*IO_Send_Func)( uint8_t *, uint16_t);
typedef int16_t (*IO_ReceiveOne_Func)(uint8_t* pSingleData);
typedef uint32_t (*App_GetTickCb_Func)(void);


typedef struct {
  uint32_t retval;
  char retstr[100];
} BG96_RetKeywords_t;

typedef enum {
  BG96_RETURN_OK            = RET_OK,         /*shall be aligned with above definitions */
  BG96_RETURN_ERROR         = RET_ERROR,      /*shall be aligned with above definitions */
  BG96_RETURN_CME_ERROR     = RET_CME_ERROR,  /*shall be aligned with above definitions */
  BG96_RETURN_RETRIEVE_ERROR = -1,
  BG96_RETURN_SEND_ERROR    = -2
}BG96_Return_t;

typedef enum {   /* See CME Error codes */
  BG96_SIM_ERROR           = 0,
  BG96_SIM_NOT_INSERTED    = 10,
  BG96_SIM_PIN_REQUIRED    = 11,
  BG96_SIM_PUK_REQUIRED    = 12,
  BG96_SIM_FAILURE         = 13,
  BG96_SIM_BUSY            = 14,
  BG96_SIM_WRONG           = 15,
  BG96_INCORRECT_PSW       = 16,
  BG96_SIM_PIN2_REQUIRED   = 17,
  BG96_SIM_PUK2_REQUIRED   = 18,
  BG96_OPERATION_NOT_ALLOW = 3,
  BG96_SIM_READY           = 0xFF
} BG96_SIMState_t;

typedef enum {
  BG96_NRS_NOT_REGISTERED  = 0x00,
  BG96_NRS_HOME_NETWORK    = 0x01,
  BG96_NRS_TRYING          = 0x02,
  BG96_NRS_REG_DENIED      = 0x03,
  BG96_NRS_UNKNOWN         = 0x04,
  BG96_NRS_ROAMING         = 0x05,
  BG96_NRS_ERROR           = 0xFF
} BG96_NetworkRegistrationState_t;

typedef enum {
  BG96_AP_NOT_CONFIG  = 0x00,
  BG96_AP_CONFIGURED  = 0x01,
  BG96_AP_ACVTIVATED  = 0x02,
  BG96_AP_ERROR       = 0xFF
} BG96_APState_t;

typedef enum {
  BG96_INIT_RET_OK        = RET_OK,       /*shall be aligned with above definitions */
  BG96_INIT_RET_AT_ERR    = 0x04,
  BG96_INIT_RET_SIM_ERR   = 0x08,        
  BG96_INIT_RET_IO_ERR    = 0x10,
  BG96_INIT_OTHER_ERR     = 0x20
}BG96_InitRet_t;

typedef enum {
  BG96_SEND_RET_UART_FAIL  = 0x1,
  BG96_SEND_RET_SENT       = RET_SENT,     /*shall be aligned with above definitions */
  BG96_SEND_RET_BUF_FULL   = RET_BUF_FULL, /*shall be aligned with above definitions */
  BG96_SEND_RET_CONN_ERR   = RET_ERROR     /*shall be aligned with above definitions */
}BG96_SendRet_t;

typedef enum {
  BG96_RECEIVE_RET_INCOMPLETE    = 0x01,
  BG96_RECEIVE_RET_OK            = RET_OK, /*shall be aligned with above definitions */
  BG96_RECEIVE_RET_PARAM_ERR     = 0x04,
  BG96_RECEIVE_RET_COM_ERR       = 0x08
}BG96_ReceiveRet_t;

typedef enum {
  BG96_TCP_CONNECTION           = 0,
  BG96_UDP_CONNECTION           = 1,
  BG96_TCP_LISTENER_CONNECTION  = 2,
  BG96_UDP_SERVER_CONNECTION    = 3
}BG96_ConnType_t;

typedef enum {
  BG96_BUFFER_MODE           = 0,
  BG96_DIRECT_PUSH           = 1,
  BG96_TRANSPARENT_MODE      = 2
}BG96_AccessMode_t;

/**
 * \brief  Authentication settings for C2C network
 */
typedef enum {
  BG96_AUTHENT_NONE     = 0x00,
  BG96_AUTHENT_PAP      = 0x01,
  BG96_AUTHENT_CHAP     = 0x02,
  BG96_AUTHENT_PAP_CHAP = 0x03
}BG96_Authent_t;

typedef enum {
  BG96_UART_FLW_CTL_NONE     = 0x00,   
  BG96_UART_FLW_CTL_RTS      = 0x01, 
  BG96_UART_FLW_CTL_CTS      = 0x02,  
  BG96_UART_FLW_CTL_RTS_CTS  = 0x03,    
} BG96_UART_FLW_CTL_t;

typedef struct
{
  uint32_t           BaudRate;          
  uint32_t           FlowControl;   
}BG96_UARTConfig_t;

typedef struct {
  uint8_t            ContextID;      /*!< range is 1-20 */
  uint8_t            ContextType;    /*!< shall be 1 (IpV */
  uint8_t            ApnString[25];  /*!< access point name, string of chars */
  uint8_t            Username[25];   /*!< user name, string of chars */
  uint8_t            Password[25];   /*!< password, string of chars */
  BG96_Authent_t     Authentication;
} BG96_APConfig_t;

typedef struct {
  BG96_ConnType_t    Type;     
  BG96_AccessMode_t  AccessMode;  
  uint8_t            ConnectID;             
  uint16_t           RemotePort;         
  uint16_t           LocalPort;          
  char*              Url;  
} BG96_Conn_t;

typedef struct {
  BG96_ConnType_t    Type;     
  BG96_AccessMode_t  AccessMode;
  uint16_t           ComulatedQirdData;
  uint16_t           HaveReadLength;
  uint16_t           UnreadLength;
  int16_t            UartRemaining; /* if Timeout respects UART speed this should always be 0 */  
} BG96_Socket_t;

typedef struct {
  IO_Init_Func       IO_Init;  
  IO_DeInit_Func     IO_DeInit;
  IO_Baudrate_Func   IO_Baudrate;
  IO_Flush_Func      IO_FlushBuffer;  
  IO_Send_Func       IO_Send;
  IO_ReceiveOne_Func IO_ReceiveOne;  
} BG96_IO_t;

typedef struct {
  BG96_SIMState_t    SimStatus;  
  uint8_t            RegistStatusString[3];  
  uint8_t            IMSI [BG96_IMSI_SIZE];  
  uint8_t            ICCID [BG96_ICCID_SIZE+1];
} BG96_SIMInfo_t;

typedef struct {
  uint8_t             Manufacturer[BG96_MFC_SIZE];  
  uint8_t             ProductID[BG96_PROD_ID_SIZE];  
  uint8_t             FW_Rev[BG96_FW_REV_SIZE];  
  uint8_t             Imei[BG96_IMEI_SIZE];               /*International Mobile Equipment Identity*/
  BG96_SIMInfo_t      SimInfo;   
  uint8_t             APsActive;
  uint8_t             APContextState[BG96_MAX_CONTEXTS];  /* to decide if keeping all BG96_APConfig_t info. maybe at c2c SW level*/  
  BG96_Socket_t       SocketInfo[BG96_MAX_SOCKETS];       /* to decide if keeping all BG96_Conn_t info. maybe at c2c SW level*/
  BG96_UARTConfig_t   UART_Config;  
  BG96_IO_t           fops;
  App_GetTickCb_Func  GetTickCb;
  uint8_t             CmdResp[BG96_CMD_SIZE];
  uint32_t            RemainRxData;
}BG96Object_t;

/* Exported functions ------------------------------------------------------- */
/* ==== Init and status ==== */

BG96_Return_t  BG96_RegisterBusIO(BG96Object_t *Obj, IO_Init_Func IO_Init,
                                                     IO_DeInit_Func IO_DeInit,
                                                     IO_Baudrate_Func IO_Baudrate,
                                                     IO_Send_Func IO_Send,
                                                     IO_ReceiveOne_Func IO_ReceiveOne,
                                                     IO_Flush_Func IO_Flush);

BG96_InitRet_t BG96_Init(BG96Object_t *Obj);

BG96_Return_t BG96_PowerDown(BG96Object_t *Obj);

/* ==== Registration and network selection ==== */

BG96_Return_t  BG96_GetSignalQualityStatus(BG96Object_t *Obj, int32_t *Qvalue);
BG96_Return_t  BG96_PSAttach(BG96Object_t *Obj);
BG96_Return_t  BG96_AutomaticPlmnSelection(BG96Object_t *Obj);
BG96_Return_t  BG96_SetFullFunctionality(BG96Object_t *Obj);
BG96_NetworkRegistrationState_t  BG96_GetCsNetworkRegistrationStatus(BG96Object_t *Obj);
BG96_NetworkRegistrationState_t  BG96_GetPsNetworkRegistrationStatus(BG96Object_t *Obj);


BG96_Return_t  BG96_ListOperators(BG96Object_t *Obj, char *Operators);
BG96_Return_t  BG96_GetCurrentOperator(BG96Object_t *Obj, char *Operator, uint8_t Bufsize);
BG96_Return_t  BG96_ForceOperator(BG96Object_t *Obj, int32_t OperatorCode);

/* ==== AP Connection ==== */

BG96_Return_t   BG96_ConfigureAP(BG96Object_t *Obj, BG96_APConfig_t *ApConfig);
BG96_Return_t   BG96_Activate(BG96Object_t *Obj, uint8_t ContextID);
BG96_Return_t   BG96_Deactivate(BG96Object_t *Obj, uint8_t ContextID);
BG96_APState_t  BG96_IsActivated(BG96Object_t *Obj, uint8_t ContextID);

/* ====IP Addr ==== */

BG96_Return_t   BG96_GetActiveIpAddresses(BG96Object_t *Obj, char *IPaddr_string, uint8_t* IPaddr_int);

/* ==== Ping ==== */

#if (BG96_USE_PING == 1)
BG96_Return_t  BG96_Ping(BG96Object_t *Obj, uint8_t ContextID, char *host_addr_string, uint16_t count, uint16_t rep_delay_sec);
#endif

/* ==== Client connection ==== */

BG96_Return_t      BG96_DNS_LookUp(BG96Object_t *Obj, uint8_t ContextID, const char *IPaddr_string, uint8_t* IPaddr_int);
BG96_Return_t      BG96_OpenClientConnection(BG96Object_t *Obj, uint8_t ContextID, BG96_Conn_t *conn);
BG96_Return_t      BG96_CloseClientConnection(BG96Object_t *Obj, BG96_Conn_t *conn);

BG96_SendRet_t     BG96_SendData(BG96Object_t *Obj, uint8_t Socket, uint8_t *pdata, uint16_t Reqlen , uint16_t *SentLen , uint32_t Timeout);
BG96_ReceiveRet_t  BG96_ReceiveData(BG96Object_t *Obj, uint8_t Socket, uint8_t *pdata, uint16_t Reqlen, uint16_t *Receivedlen, uint32_t Timeout);

/* ==== Miscellaneus ==== */

BG96_Return_t  BG96_ResetToFactoryDefault(BG96Object_t *Obj);
BG96_Return_t  BG96_SetUARTBaudrate(BG96Object_t *Obj, BG96_UARTConfig_t *pconf);
BG96_Return_t  BG96_GetUARTConfig(BG96Object_t *Obj, BG96_UARTConfig_t *pconf);

void           BG96_GetManufacturer( BG96Object_t *Obj, uint8_t *Manufacturer);
void           BG96_GetProductID(BG96Object_t *Obj, uint8_t *ProductID);
void           BG96_GetFWRevID(BG96Object_t *Obj, uint8_t *Fw_ver);

BG96_Return_t  BG96_RetrieveLastErrorDetails(BG96Object_t *Obj, char *error_string);

/* Application must provide callback function that gives a Timer Tick in ms (e.g. HAL_GetTick())*/
BG96_Return_t  BG96_RegisterTickCb(BG96Object_t *Obj, App_GetTickCb_Func  GetTickCb);


#ifdef __cplusplus
}
#endif

#endif/* __BG96_H__ */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/ 
