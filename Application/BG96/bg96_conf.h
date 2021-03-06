/**
  ******************************************************************************
  * @file    ug96_conf.h
  * @author  MCD Application Team
  * @brief   UG96 configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
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

#ifndef __BG96_CONF_H
#define __BG96_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif  

/* --------------------------------------------------------------------------- */   
/* UG96 static parameter that can be configured by the user before compilation */ 
/* --------------------------------------------------------------------------- */   

/* Max sockets allowed by modem is 12 but beter use max 9 (sw not tested above) */   
#define BG96_MAX_SOCKETS                        1
/* Max context allowed by modem is 20 but beter use max 9 (sw not tested above) */   
#define BG96_MAX_CONTEXTS                       1

/* 256 is normally sufficient, but if some params like URL are very long string it can be increased*/
#define BG96_CMD_SIZE                           256                                                      

/* Ug96 and STM32 shall have same baudrate to communicate */
#define BG96_DEFAULT_BAUDRATE                   115200 

/* Rx and Tx buffer size, depend as the applic handles the buffer */
#define BG96_TX_DATABUF_SIZE                    1460 /* mbedTLS needs at least 1460 */
#define BG96_RX_DATABUF_SIZE                    1500

/* 0 to disable, 1 to enable PING feature */
#define BG96_USE_PING                           1

#ifdef __cplusplus
}
#endif
#endif /* __BG96_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
