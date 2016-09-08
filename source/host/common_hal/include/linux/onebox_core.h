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

uint8 coex_get_queue_num(PONEBOX_ADAPTER adapter);
void coex_pkt_processor(PONEBOX_ADAPTER adapter);
inline void coex_queue_pkt(PONEBOX_ADAPTER adapter,netbuf_ctrl_block_t *netbuf_cb, 
                           uint8 q_num);
void onebox_print_dump(int32 dbg_zone_l, PUCHAR msg_to_print_p,int32 len);
inline netbuf_ctrl_block_t* coex_dequeue_pkt(PONEBOX_ADAPTER adapter, uint8 q_num);
#endif
