
/**
 * @file    rsi_http.c
 * @version 0.1
 * @date    15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains HTTP-GET/POST API
 *
 *  @section Description  This file contains HTTP-GET/POST API
 *
 *
 */
//! Include Driver header file
#include "rsi_driver.h"

//! Include Application header file
#include "rsi_http_client.h"

/*
 * Global Variables
 * */
extern rsi_driver_cb_t *rsi_driver_cb;

/*==============================================*/
/**
 * @fn         int32_t rsi_http_client_get_async(uint8_t flags, uint8_t *ip_address, uint16_t port_no, uint8_t *resource,
                                                uint8_t *host_name, uint8_t *extended_header, uint8_t *user_name, uint8_t *password,
                                                void(*http_client_get_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata))
 * @brief      To get the requested http page of given url 
 * @param[in]  flags,  to select version and security , BIT(0) : 0 - IPv4 , 1 - IPv6, BIT(1): 0 - HTTP , 1 - HTTPS
 * @param[in]  ip_address, server IP address
 * @param[in]  port_no, port number : 80 - HTTP , 443 - HTTPS
 * @param[in]  resource, URL string for requested resource
 * @param[in]  host_name, host name
 * @param[in]  extended_header, extender header if present
 * @param[in]  username, username for server Authentication
 * @param[in]  password, password for server Authentication
 * @param[in]  http_client_get_response_handler, call back when asyncronous response comes for the request 
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to get the http data for the requested URL 
 *
 */

int32_t rsi_http_client_get_async(uint8_t flags, uint8_t *ip_address, uint16_t port, 
                            uint8_t *resource, uint8_t *host_name,
                            uint8_t *extended_header, uint8_t *user_name, uint8_t *password,
                            void(*http_client_get_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata))
{
  return rsi_http_client_async(RSI_HTTP_GET, flags, ip_address, port, resource, host_name, extended_header, user_name, password, NULL, 0, http_client_get_response_handler);

}


/*==============================================*/
/**
 * @fn         int32_t rsi_http_client_post(uint8_t flags, uint8_t *ip_address, uint16_t port, uint8_t *resource, uint8_t *host_name,
 *                                          uint8_t *extended_header, uint8_t *user_name, uint8_t *password,uint8_t *post_data, uint16_t post_data_length,
                                            void(*http_client_get_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata))
 * @brief      To get the requested http page of given url 
 * @param[in]  flags,  to select version and security , BIT(0) : 0 - IPv4 , 1 - IPv6, BIT(1): 0 - HTTP , 1 - HTTPS
 * @param[in]  ip_address, server IP address
 * @param[in]  port, port number : 80 - HTTP , 443 - HTTPS, non standard ports are also allowed
 * @param[in]  resource, URL string for requested resource
 * @param[in]  host_name, host name
 * @param[in]  extended_header, extender header if present
 * @param[in]  username, username for server Authentication
 * @param[in]  password, password for server Authentication
 * @param[in]  post_data, http data to be posted to server
 * @param[in]  post_data_length, http data length to be posted to server
 * @param[in]  http_client_post_response_handler, call back when asyncronous response comes for the request 
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to post the http data for the requested URL to http server 
 *
 */
int32_t rsi_http_client_post_async(uint8_t flags, uint8_t *ip_address, uint16_t port, 
                            uint8_t *resource, uint8_t *host_name, uint8_t *extended_header, 
                            uint8_t *user_name, uint8_t *password, uint8_t *post_data, uint16_t post_data_length,
                            void(*http_client_post_response_handler)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata))
{
  return rsi_http_client_async(RSI_HTTP_POST, flags, ip_address, port, resource, host_name, extended_header, user_name, password, post_data, post_data_length, http_client_post_response_handler);

}


/*==============================================*/
/**
 * @fn         int32_t rsi_http_client_async(uint8_t type, uint8_t flags, uint8_t *ip_address, uint16_t port, uint8_t *resource, uint8_t *host_name,
                                             uint8_t *extended_header, uint8_t *user_name, uint8_t *password,uint8_t *post_data, uint16_t post_data_length,
                                             void(*callback)(uint8_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata))
 * @brief      To get the requested http page of given url 
 * @param[in]  type, 0 - HTTPGET , 1 - HTTPPOST
 * @param[in]  flags,  to select version and security , BIT(0) : 0 - IPv4 , 1 - IPv6, BIT(1): 0 - HTTP , 1 - HTTPS
 * @param[in]  ip_address, server IP address
 * @param[in]  port, port number : 80 - HTTP , 443 - HTTPS
 * @param[in]  resource, URL string for requested resource
 * @param[in]  host_name, host name
 * @param[in]  extended_header, extender header if present
 * @param[in]  username, username for server Authentication
 * @param[in]  password, password for server Authentication
 * @param[in]  post_data, http data to be posted to server
 * @param[in]  post_data_length, http data length to be posted to server
 * @param[in]  callback, call back when asyncronous response comes for the request 
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to post the http data for the requested URL to http server 
 *
 */

int32_t rsi_http_client_async(uint8_t type, uint8_t flags, uint8_t *ip_address, uint16_t port, 
                            uint8_t *resource, uint8_t *host_name, uint8_t *extended_header, 
                            uint8_t *user_name, uint8_t *password, uint8_t *post_data, uint16_t post_data_length,
                            void(*callback)(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata))
{
  rsi_req_http_client_t     *http_client;
  rsi_pkt_t                 *pkt;
  int32_t                    status = RSI_SUCCESS;
  uint8_t                    https_enable = 0;
  uint16_t                   http_length = 0; 
  uint16_t                   send_size = 0; 
  uint8_t                   *host_desc = NULL;


  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! if state is not in ipconfig done state
  if((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE))
  {
    //! Command given in wrong state 
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  //! take lock on wlan control block
  rsi_mutex_lock(&wlan_cb->wlan_mutex);

  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&wlan_cb->wlan_mutex);

    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! register callback
  if(callback != NULL)
  {
    //! Register http client response notify call back handler
    rsi_driver_cb->nwk_cb->nwk_callbacks.http_client_response_handler = callback;
  }
  else
  {
    //! return invalid command error
    return RSI_ERROR_INVALID_PARAM;

  }

  http_client = (rsi_req_http_client_t *) pkt->data;

  //! memset the packet data to insert NULL between fields
  memset(&pkt->data, 0, sizeof(rsi_req_http_client_t));

  //! Fill ipversion 
  if(flags & RSI_IPV6)
  {  
    //! Set ipv6 version 
    rsi_uint16_to_2bytes(http_client->ip_version, RSI_IP_VERSION_6);
  }
  else
  {
    //! Set ipv4 version 
    rsi_uint16_to_2bytes(http_client->ip_version, RSI_IP_VERSION_4);

  }
  if(flags & RSI_SSL_ENABLE)
  {
    //! Set https feature
    https_enable = 1;

    //! set default by NULL delimiter
    https_enable |= BIT(1);

    //! Fill https features parameters
    rsi_uint16_to_2bytes(http_client->https_enable, https_enable);
  }

  //! Fill port no
  http_client->port = port;
  
  //! Copy username
  rsi_strcpy(http_client->buffer, user_name);
  http_length = rsi_strlen(user_name) + 1;

  //! Copy  password
  rsi_strcpy((http_client->buffer) + http_length, password);
  http_length += rsi_strlen(password) + 1;

  //! Copy  Host name
  rsi_strcpy((http_client->buffer) + http_length, host_name);
  http_length += rsi_strlen(host_name) + 1;

  //! Copy IP address
  rsi_strcpy((http_client->buffer) + http_length, ip_address);
  http_length += rsi_strlen(ip_address) + 1;

  //! Copy URL resource
  rsi_strcpy((http_client->buffer) + http_length, resource);
  http_length += rsi_strlen(resource) + 1;

  if(extended_header != NULL)
  {
  
    //! Copy Extended header
    rsi_strcpy((http_client->buffer) + http_length, extended_header);
    http_length += rsi_strlen(extended_header);
  }

  if(type)
  {
    //! Copy Httppost data
    memcpy((http_client->buffer) + http_length + 1, post_data, post_data_length);

    http_length += (post_data_length + 1);
  }

  //! Using host descriptor to set payload length
  send_size = sizeof(rsi_req_http_client_t) - RSI_HTTP_BUFFER_LEN + http_length;

  //! get the host descriptor
  host_desc = (pkt->desc);

  //! Fill data length in the packet host descriptor
  rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

  //! Magic number to set asyncronous reswponse
  wlan_cb->expected_response = RSI_WLAN_RSP_ASYNCHRONOUS ;


  if(type)
  {
    //! send HTTP Post request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_CLIENT_POST, pkt);
  }
  else
  {
    //! send HTTP Get request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_CLIENT_GET, pkt);
  }
  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status
  return status;

}


/*==============================================*/
/**
 * @fn         int32_t rsi_http_client_abort(void)
 * @brief      To abort if any http client request is in progress
 * @param[in]  none
 * @param[out] none
 * @return     int32_t 
 *             0  =  success
 *             <0 = failure 
 * @section description
 * This function is used to abort any on going http request 
 *
 */

int32_t rsi_http_client_abort(void)
{
  rsi_pkt_t       *pkt;
  int8_t          status = RSI_SUCCESS;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;


  //! take lock on wlan control block
  rsi_mutex_lock(&wlan_cb->wlan_mutex);

  //! allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

  //! If allocation of packet fails
  if(pkt == NULL)
  {
    //! unlock mutex
    rsi_mutex_unlock(&wlan_cb->wlan_mutex);

    //! return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  //! send join command to start wps 
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_ABORT, pkt);

  //! free the packet
  rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);

  //! unlock mutex
  rsi_mutex_unlock(&wlan_cb->wlan_mutex);

  //! Return the status if error in sending command occurs
  return status;
}
