/**
 * @file onebox_core.h
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
 * This file contians the function prototypes used in the core module
 * 
 */
#ifndef __ONEBOX_CORE_H__
#define __ONEBOX_CORE_H__

#include "onebox_common.h"

/*onebox_core_bt.c function declarations*/
int32 bt_core_pkt_recv(BT_ADAPTER bt_adapter, netbuf_ctrl_block_t *netbuf_cb);
int32 core_bt_init(BT_ADAPTER bt_adapter);
int32 core_bt_deinit(BT_ADAPTER bt_adapter);
int32 bt_xmit(BT_ADAPTER bt_adapter, netbuf_ctrl_block_t *netbuf_cb);
int32 bt_read_reg_params(BT_ADAPTER bt_adapter);
int32 bt_cw(BT_ADAPTER bt_adapter, uint8 *data, int32 len);
int32 bt_ber(BT_ADAPTER bt_adapter, uint8 *data, int32 len);
int core_bt_deque_pkts(BT_ADAPTER bt_adapter);
void onebox_print_dump(int32 dbg_zone_l, PUCHAR msg_to_print_p,int32 len);

#endif
