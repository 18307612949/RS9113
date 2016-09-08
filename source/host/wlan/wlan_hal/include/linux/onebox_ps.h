/**
 * @file onebox_ps.h
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
 * This file contians the data structures and variables/ macros commonly
 * used in the driver .
 */
#ifndef __ONEBOX_PS_H__
#define __ONEBOX_PS_H__
#include "wlan_common.h"

#define DEEP_SLEEP		1
#define CONNECTED_SLEEP		2

#define PS_ENABLE 1
#define PS_DISABLE 0
#if 0
#define CONFIRM_PATH 1
#define MGMT_PENDING_PATH 2
#define IOCTL_PATH 3
#define CONNECTED_PATH 4
#define DISCONNECTED_PATH 5
#endif

enum ps_paths {
MGMT_PENDING_PATH = 1,//will handle both scan_start and any mgmt pending in DEEP_SLEEP pwr_save state 
IOCTL_PATH,
CONNECTED_PATH,
DISCONNECTED_PATH,
SCAN_END_PATH,
TIMER_PATH,
VAP_CREATE_PATH,
VAP_DELETE_PATH,
TX_RX_PATH,
PS_EN_DEQUEUED_PATH,
PS_EN_REQ_CNFM_PATH,
PS_DIS_REQ_CNFM_PATH
};



void update_pwr_save_status(struct ieee80211vap *vap, uint32 ps_status, uint32 path);
void support_mimic_uapsd(WLAN_ADAPTER w_adapter, unsigned char *buffer);

#endif
