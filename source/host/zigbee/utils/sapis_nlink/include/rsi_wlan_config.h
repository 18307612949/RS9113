/**
 * @file         rsi_config.h
 * @version      0.1
 * @date         15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains user configurable details to configure the device  
 *
 *  @section Description  This file contains user configurable details to configure the device 
 *
 *
 */
#ifndef RSI_CONFIG_H
#define RSI_CONFIG_H

#include "rsi_wlan_defines.h"

//! Enable feature
#define ENABLE                 1
//! Disable feature
#define DISABLE                0

//! To enable concurrent mode
#define CONCURRENT_MODE                      DISABLE

//! opermode command paramaters
/*=======================================================================*/

//! To set wlan feature select bit map
#define RSI_FEATURE_BIT_MAP          (FEAT_SECURITY_OPEN )

//! TCP IP BYPASS feature check
#define RSI_TCP_IP_BYPASS            DISABLE

//! TCP/IP feature select bitmap for selecting TCP/IP features    
#define RSI_TCP_IP_FEATURE_BIT_MAP  ( TCP_IP_FEAT_MDNSD |	TCP_IP_FEAT_HTTP_SERVER | TCP_IP_FEAT_DHCPV4_CLIENT)

//! To set custom feature select bit map 
#define RSI_CUSTOM_FEATURE_BIT_MAP   0

/*=======================================================================*/




//! Band command paramters
/*=======================================================================*/

//! RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_BAND                    RSI_BAND_2P4GHZ            

/*=======================================================================*/



//! set region command paramters
/*=======================================================================*/

//! ENABLE or DISABLE Set region support
#define  RSI_SET_REGION_SUPPORT             DISABLE                 //@ ENABLE or DISABLE set region

//! If 1:region configurations taken from user ;0:region configurations taken from beacon
#define RSI_SET_REGION_FROM_USER_OR_BEACON 1 

//! 0-Default Region domain ,1-US, 2-EUROPE, 3-JAPAN
#define RSI_REGION_CODE                    1   

/*=======================================================================*/


//! set region AP command paramters
/*=======================================================================*/

//! ENABLE or DISABLE Set region AP support
#define RSI_SET_REGION_AP_SUPPORT          DISABLE

//! If 1:region configurations taken from user ;0:region configurations taken from firmware
#define RSI_SET_REGION_AP_FROM_USER        ENABLE    

//! "US" or "EU" or "JP" or other region codes
#define RSI_COUNTRY_CODE               "US "       


/*=======================================================================*/


//! Rejoin parameters 
/*=======================================================================*/

//! ENABLE or DISABLE rejoin params
#define RSI_REJOIN_PARAMS_SUPPORT       DISABLE

//! Rejoin retry count. If 0 retries infinity times
#define RSI_REJOIN_MAX_RETRY           0         

//! Periodicity of rejoin attempt
#define RSI_REJOIN_SCAN_INTERVAL       4   

//! Beacon missed count
#define RSI_REJOIN_BEACON_MISSED_COUNT   40                    

//! ENABLE or DISABLE retry for first time join failure
#define RSI_REJOIN_FIRST_TIME_RETRY    DISABLE            

/*=======================================================================*/



//!BG scan command parameters
/*=======================================================================*/

//! ENABLE or DISABLE BG Scan support
#define RSI_BG_SCAN_SUPPORT     DISABLE

//! ENABLE or DISABLE BG scan
#define RSI_BG_SCAN_ENABLE             ENABLE       

//! ENABLE or DISABLE instant BG scan
#define RSI_INSTANT_BG                 DISABLE      

//! BG scan threshold value
#define RSI_BG_SCAN_THRESHOLD          10               

//! RSSI tolerance Threshold
#define RSI_RSSI_TOLERANCE_THRESHOLD   4            

//! BG scan periodicity 
#define RSI_BG_SCAN_PERIODICITY        10    

//! Active scan duration
#define RSI_ACTIVE_SCAN_DURATION       15          

//! Passive scan duration
#define RSI_PASSIVE_SCAN_DURATION      20          

//! Multi probe 
#define RSI_MULTIPROBE                 DISABLE

/*=======================================================================*/




//!Roaming parameters
/*=======================================================================*/

//! ENABLE or DISABLE Roaming support
#define RSI_ROAMING_SUPPORT         DISABLE

//! roaming threshold value 
#define RSI_ROAMING_THRESHOLD          10       

//! roaming hysterisis value
#define RSI_ROAMING_HYSTERISIS         4      

/*=======================================================================*/


//! High Throughput Capabilies related information
/*=======================================================================*/

//! ENABLE or DISABLE 11n mode in AP mode
#define RSI_MODE_11N_ENABLE            DISABLE

//! HT caps bit map.
#define RSI_HT_CAPS_BIT_MAP            10             

/*=======================================================================*/




//! Scan command parameters
/*=======================================================================*/

//! scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#define RSI_SCAN_CHANNEL_BIT_MAP_2_4    0                     

//! scan channle bit map in 5GHz band ,valid if given channel to scan is 0
#define RSI_SCAN_CHANNEL_BIT_MAP_5      0                    

//! scan_feature_bitmap ,valid only if specific channel to scan and ssid are given
#define RSI_SCAN_FEAT_BITMAP            0                           

/*=======================================================================*/




//! Enterprise configuration command parameters
/*=======================================================================*/

//! Enterprise method ,should be one of among TLS, TTLS, FAST or PEAP
#define RSI_EAP_METHOD                   "TTLS"
//! This parameter is used to configure the module in Enterprise security mode
#define RSI_EAP_INNER_METHOD             "\"auth=MSCHAPV2\"" 

/*=======================================================================*/




//! AP configuration command parameters
/*=======================================================================*/

//! This Macro is used to enable AP keep alive functionality
#define RSI_AP_KEEP_ALIVE_ENABLE        ENABLE                   

//! This parameter is used to configure keep alive type
#define RSI_AP_KEEP_ALIVE_TYPE          RSI_NULL_BASED_KEEP_ALIVE          

//! This parameter is used to configure keep alive period
#define RSI_AP_KEEP_ALIVE_PERIOD        100         

//! This parameter is used to configure maximum stations supported
#define RSI_MAX_STATIONS_SUPPORT         4 
/*=======================================================================*/





//! Join command parameters
/*=======================================================================*/

//! Tx power level
#define RSI_POWER_LEVEL            RSI_POWER_LEVEL_HIGH      

//! RSI_JOIN_FEAT_STA_BG_ONLY_MODE_ENABLE or RSI_JOIN_FEAT_LISTEN_INTERVAL_VALID
#define RSI_JOIN_FEAT_BIT_MAP	   0						  

//! 
#define RSI_LISTEN_INTERVAL		   0

//! Transmission data rate. Physical rate at which data has to be transmitted.
#define RSI_DATA_RATE                   RSI_DATA_RATE_AUTO         

/*=======================================================================*/



//! Transmit test command parameters
/*=======================================================================*/
//! TX TEST rate flags
#define RSI_TX_TEST_RATE_FLAGS        0        

//! TX TEST per channel bandwidth
#define RSI_TX_TEST_PER_CH_BW         0      

//! TX TEST aggregation enable or disable
#define RSI_TX_TEST_AGGR_ENABLE       DISABLE    

//! TX TEST delay
#define RSI_TX_TEST_DELAY             0          

/*=======================================================================*/


//! ssl parameters
/*=======================================================================*/
//! ssl version
#define RSI_SSL_VERSION               0        

//! ssl ciphers
#define RSI_SSL_CIPHERS               SSL_ALL_CIPHERS     

//! Power save command parameters
/*=======================================================================*/
//! set handshake type of power mode
#define RSI_HAND_SHAKE_TYPE                       MSG_BASED

//! 0 - LP, 1- ULP mode with RAM retention and 2 - ULP with Non RAM retention
#define RSI_SELECT_LP_OR_ULP_MODE                 RSI_LP_MODE

//! set DTIM aligment required
//! 0 - module wakes up at beacon which is just before or equal to listen_interval
//! 1 - module wakes up at DTIM beacon which is just before or equal to listen_interval
#define RSI_DTIM_ALIGNED_TYPE                     0

//! Monitor interval for the FAST PSP mode
//! default is 50 ms, and this parameter is valid for FAST PSP only
#define RSI_MONITOR_INTERVAL                      50

//!WMM PS parameters
//! set wmm enable or disable
#define RSI_WMM_PS_ENABLE                         DISABLE

//! set wmm enable or disable
//! 0- TX BASED 1 - PERIODIC
#define RSI_WMM_PS_TYPE                           0

//! set wmm wake up interval
#define RSI_WMM_PS_WAKE_INTERVAL                  20

//! set wmm UAPSD bitmap
#define RSI_WMM_PS_UAPSD_BITMAP                   15

/*=======================================================================*/

#endif

