
/** @file hal/micro/cortexm3/uart.h
 * @brief Header for STM32W  uart drivers, supporting IAR's standard library
 *        IO routines.
 *
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */

#ifndef __UART_MIN_H__
#define __UART_MIN_H__
#ifdef __ICCARM__
#include <yfuns.h>
#endif
#include "ZigBeeDataTypes.h"
#include "common_includes.h"
#ifdef	STM32L1XX_MD_PLUS
#include "stm32l1xx.h"
#endif
/**
 * @brief A list of the possible values for the parity parameter to uartInit()
 */
typedef enum
{
  PARITY_NONE = 0,
  PARITY_ODD = 1,
  PARITY_EVEN = 2
} SerialParity;

#ifndef UART_CONFIG
  #ifndef STM32L1XX_MD_PLUS
    #define UART_BAUD_RATE  115200u
    #define UART_DATA_BITS  8u
    #define UART_PARITY     PARITY_NONE
    #define UART_STOP_BITS  1u
  #else
    #define UART_BAUD_RATE  115200u
    #define UART_DATA_BITS  0u
    #define UART_PARITY     0u
    #define UART_STOP_BITS  0u
  #endif
#endif /* UART_CONFIG */

/**
 * @brief Initialize the UART
 *
 * @param baudrate  The baudrate which will be used for communication.
 *                  Ex: 115200
 *
 * @param databits  The number of data bits used for communication.
 *                  Valid values are 7 or 8
 *
 * @param parity    The type of parity used for communication.
 *                  See the SerialParity enum for possible values
 *
 * @param stopbits The number of stop bits used for communication.
 *                  Valid values are 1 or 2
 */
//void uart_init(uint32_t baudrate, uint8_t databits, SerialParity parity, uint8_t stopbits);

#ifdef __ICCARM__
/**
 * @brief Flush the output stream.  DLib_Config_Full.h defines fflush(), but
 * this library includes too much code so we compile with DLib_Config_Normal.h
 * instead which does not define fflush().  Therefore, we manually define
 * fflush() in the low level UART driver.  This function simply redirects
 * to the __write() function with a NULL buffer, triggering a flush.
 *
 * @param handle  The output stream.  Should be set to 'stdout' like normal.
 *
 * @return Zero, indicating success.
 */
size_t fflush(int handle);

/**
 * @brief Define the stdout stream.  Since we compile with DLib_Config_Normal.h
 * it does not define 'stdout'.  There is a low-level IO define '_LLIO_STDOUT'
 * which is equivalent to stdout.  Therefore, we define 'stdout' to be
 * '_LLIO_STDOUT'.
 */
#define stdout _LLIO_STDOUT
#endif
/**
 * @brief Read the input byte if any.
 */
bool __io_getcharNonBlocking(uint8_t *data);
void __io_putchar( char c );
int __io_getchar(void);
void __io_flush( void );

void uart_deInit (void);

#endif //__UART_MIN_H__