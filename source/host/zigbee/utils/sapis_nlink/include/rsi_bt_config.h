/**
 * @file     rsi_bt_config.h
 * @version  0.1  
 * @date     03 Sep 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contain definitions and declarations of BT APIs.
 *
 *  @section Description  This file contains definitions and declarations required to
 *  configure BT module.
 *
 *
 */

#ifndef RSI_BT_CONFIG_H
#define RSI_BT_CONFIG_H

#include<rsi_data_types.h>

/******************************************************
 * *                      Macros
 * ******************************************************/

//! WLAN + BT Classic coex mode
#define RSI_OPERMODE_WLAN_BT_CLASSIC                5

/*=======================================================================*/
//! Discovery command parameters
/*=======================================================================*/

//! Discovery mode
//! Start the discoverable mode
#define START_DISCOVERY                             0x01
//! Start the limited discoverable mode
#define START_LIMITED_DISCOVERY                     0x02
//! Stop the discoverable mode
#define STOP_DISCOVERY                              0x00


/*=======================================================================*/
//! Connectability command parameters
/*=======================================================================*/

//! Connectable mode
//! Start the connectable mode
#define CONNECTABLE                                  0x01
//! Stop the connectable mode
#define NON_CONNECTABLE                              0x00


/*=======================================================================*/
//!  Authentication command parameters
/*=======================================================================*/

//! Pair mode
//! Enable authentication
//#define ENABLE                                  0x01
//! Enable authentication
//#define DISABLE                                 0x00


/*=======================================================================*/
//!  SSP Confirmation command parameters
/*=======================================================================*/

//! Confimation type
//! Accept confirmation
#define ACCEPT                                  0x01
//! Reject confirmation
#define REJECT                                  0x00


/*=======================================================================*/
#endif
