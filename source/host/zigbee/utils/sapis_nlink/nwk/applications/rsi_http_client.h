/**
 * @file     rsi_http_client.h
 * @version  0.1
 * @date     15 Aug 2015 
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains http client related information
 *
 *  @section Description  This file contains http client relating information
 *
 *
 */
#ifndef RSI_HTTP_CLIENT_H
#define RSI_HTTP_CLIENT_H
/******************************************************
 * *                      Macros
 * ******************************************************/

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/


/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_http_client_get_async(uint8_t flags, uint8_t *ip_address, uint16_t port, 
                            uint8_t *resource, uint8_t *host_name,
                            uint8_t *extended_header, uint8_t *user_name, uint8_t *password,
                            void(*http_client_get_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata));
int32_t rsi_http_client_post_async(uint8_t flags, uint8_t *ip_address, uint16_t port, 
                            uint8_t *resource, uint8_t *host_name, uint8_t *extended_header, 
                            uint8_t *user_name, uint8_t *password, uint8_t *post_data, uint16_t post_data_length,
                            void(*http_client_post_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata));

int32_t rsi_http_client_abort(void);
#endif
