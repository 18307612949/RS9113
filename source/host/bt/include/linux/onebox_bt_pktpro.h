
/**
 * @file onebox_pktpro.h
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
 * This file contians the function prototypes used for sending /receiving packets to/from the
 * driver
 * 
 */

#ifndef __ONEBOX_PKTPRO_H__
#define __ONEBOX_PKTPRO_H__

#include "onebox_datatypes.h"
#include "onebox_common.h"

int32 send_bt_pkt(BT_ADAPTER bt_adapter, netbuf_ctrl_block_t *netbuf_cb);
ONEBOX_STATUS device_init(BT_ADAPTER bt_adapter);
ONEBOX_STATUS device_deinit(BT_ADAPTER bt_adapter);
void print_hci_cmd_type(BT_ADAPTER bt_adapter, netbuf_ctrl_block_t *netbuf_cb);
void print_rx_hci_cmd_type(BT_ADAPTER bt_adapter, uint16 ogcf, uint8 pkt_type);

#endif

