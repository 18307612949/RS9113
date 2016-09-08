
/**
 * @file   onebox_cm_pwr_mgr.h
 * @author 
 * @version 1.0
 *
 * @section LICENSE
 *
 * This software embodies materials and concepts that are confidential to Redpine
 * Signals and is made available solely pursuant to the terms of a written license
 * agreement with Redpine Signals
 *
 * @section DESCRIPTION
 *
 * The file contains the declarations of common hal power manager 
 * power save across different protocols.
 */

#include "onebox_common.h"

#define GPIO_HIGH 1
#define GPIO_LOW 0

#define PROTOCOLS_TX_ACCESS if((!WLAN_TECH.tx_access) && (!BT_TECH.tx_access) && (!ZB_TECH.tx_access)) { \
                              return 0; }\
                            else { \
                             return 1; }\

void update_tx_status(struct driver_assets *d_assets, uint8 proto_id);
int protocol_tx_access(struct driver_assets *d_assets);
