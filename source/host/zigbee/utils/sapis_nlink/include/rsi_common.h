/**
 * @file        rsi_common.h
 * @version     0.1 
 * @date        15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief :  This file contains scheduler functionality defines and function declarations 
 *            of common control block used in the driver
 *  
 *  @section Description  This file contains scheduler functionality defines and function declarations 
 *            of common control block used in the driver
 *
 */

#ifndef RSI_COMMON_H
#define RSI_COMMON_H
/******************************************************
 * *                      Macros
 * ******************************************************/

//! Power save SLP and WKP frame type
#define RSI_RSP_SLP                        0xDE
#define RSI_RSP_WKP                        0xDD

/******************************************************/

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

//! enumeration for states used in common control block in driver
typedef enum rsi_common_state_e
{
  RSI_COMMON_STATE_NONE = 0,
  RSI_COMMON_CARDREADY,
  RSI_COMMON_OPERMODE_DONE
}rsi_common_state_t;

//! enumeration for command responses used in common control block
typedef enum rsi_common_cmd_response_e
{
  RSI_COMMON_RSP_CLEAR            = 0x00,	
  RSI_COMMON_RSP_OPERMODE         = 0x10,
  RSI_COMMON_RSP_ANTENNA_SELECT   = 0x1B,
  RSI_COMMON_RSP_SOFT_RESET       = 0x1C,
  RSI_COMMON_RSP_CARDREADY        = 0x89,
  RSI_COMMON_RSP_PWRMODE          = 0x15,
  RSI_COMMON_RSP_ASYNCHRONOUS     = 0xFF

}rsi_common_cmd_response_t;


//! enumeration for command request used in common control block
typedef enum rsi_common_cmd_request_e
{
  RSI_COMMON_REQ_OPERMODE               = 0x10,
  RSI_COMMON_REQ_ANTENNA_SELECT         = 0x1B,
  RSI_COMMON_REQ_SOFT_RESET             = 0x1C,
  RSI_COMMON_REQ_PWRMODE                = 0x15
}rsi_common_cmd_request_t;

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

//! opermode command request structure
typedef struct rsi_opermode_s
{
  //! operating mode 0-client, 1-p2p, 2-EAP, 6-AP, 8-PER
  uint8_t    opermode[4];                       

  //! BIT(0)-Open mode security, BIT(1)-PSK security, BIT(2) JSON objects
  uint8_t    feature_bit_map[4];

  //! BIT(0)-tcp/ip bypass, BIT(1)-HTTP server,BIT(2)-DHCPV4 client, 
  uint8_t    tcp_ip_feature_bit_map[4];          

  //! BIT(3)-DHCPV6 client, BIT(4)-DHCPV4 server, BIT(5)-DHCPV6 server
  uint8_t   custom_feature_bit_map[4]; 
} rsi_opermode_t;

//! Antenna select command request structure
typedef struct rsi_antenna_select_s 
{
        //! Antenna value to set    
        uint8_t    antenna_value;
        //! Antenna 2G gain value    
        uint8_t   gain_2g; 
        //! Antenna 5G gain value    
        uint8_t   gain_5g; 

} rsi_antenna_select_t;


/******************************************************
 * *                    Structures
 * ******************************************************/

//! driver power save structure
typedef struct rsi_power_save_s
{
//! wlan driver power save profile mode 
  uint8_t wlan_psp_mode;

  //! bt driver power save profile mode 
  uint8_t bt_psp_mode;

  //! wlan_driver power save profile type
  uint8_t wlan_psp_type;

  //! bt_driver power save profile type
  uint8_t bt_psp_type;


  //! module power save state
  uint32_t module_state;

//! Module power save State Machine 
#define RSI_IDLE               0
#define RSI_SLP_RECEIVED       1
#define RSI_SLP_ACK_SENT       2
#define RSI_WKUP_RECEIVED      3  

  //! This is used to detect current module power mode is executing
  uint8_t current_ps_mode;

  //! This is used to detect current power save mode profile type
  uint8_t power_save_enable;
}rsi_power_save_t;


//! driver common block structure
typedef struct rsi_common_cb_s
{
  //! driver common block state 
  rsi_common_state_t state;

  //! driver common block status
  volatile int32_t status;

  //! driver common block mutex
  rsi_mutex_handle_t common_mutex;

  //! driver common block command expected response
  rsi_common_cmd_response_t expected_response;

  //! driver common block semaphore
  rsi_semaphore_handle_t common_sem;

  //! driver common block tx pool
  rsi_pkt_pool_t  common_tx_pool;

  //! power save backups
  rsi_power_save_t power_save;

  //! coex mode variable for power save command
  uint16_t ps_coex_mode;
}rsi_common_cb_t;

/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_driver_process_common_recv_cmd(rsi_pkt_t *pkt);
int32_t rsi_driver_common_send_cmd(rsi_common_cmd_request_t cmd, rsi_pkt_t *pkt);
int8_t rsi_common_cb_init(rsi_common_cb_t *common_cb);
void rsi_handle_slp_wkp(uint8_t frame_type);
void rsi_req_wakeup(void);
void rsi_wait4wakeup(void);
void rsi_allow_sleep(void);
void rsi_powersave_gpio_init(void);
void rsi_common_packet_transfer_done(rsi_pkt_t *pkt);
int32_t rsi_sleep_mode_decision(rsi_common_cb_t *rsi_common_cb);
#endif
