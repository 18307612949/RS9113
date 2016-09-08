/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : serial_utils.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : November 2009
* Description        : Header file x for serial utilities
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef _SERIAL_UTILS_H_
#define _SERIAL_UTILS_H_

/* Private define ------------------------------------------------------------*/

typedef bool  (printfFlushHandler)(uint8_t *contents,
                                      uint8_t length);


/* Functions Prototypes ------------------------------------------------------*/

uint8_t *writeHex(uint8_t *charBuffer, uint16_t value, uint8_t charCount);
bool serialWriteString(const char * string);
bool serialWaitSend(void);
bool serialReadPartialLine(char *data, uint8_t max, uint8_t * index);
bool serialWriteData(uint8_t *data, uint8_t length);
bool serialWriteByte(uint8_t dataByte);
bool serialReadByte(uint8_t *dataByte);
bool SerialWriteHex(uint8_t dataByte);

#endif /* _SERIAL_UTILS_H_*/

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
