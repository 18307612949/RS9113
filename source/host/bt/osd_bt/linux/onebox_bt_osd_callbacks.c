/**
 * @file
 * @author  Sowjanya
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
 * This file contains all the function pointer initializations for os
 * interface.
 */
#include "bt_common.h"
#include "onebox_linux.h"

static struct onebox_bt_osd_operations bt_osd_ops = {
#ifdef USE_BLUEZ_BT_STACK
	.onebox_btstack_init   = bluez_init,
	.onebox_btstack_deinit = bluez_deinit,
 	.onebox_send_pkt_to_btstack = send_pkt_to_bluez,
#endif
	.onebox_genl_init   = btgenl_init,
	.onebox_genl_deinit = btgenl_deinit,
 	.onebox_send_pkt_to_genl = send_pkt_to_btgenl,
#ifdef RSI_CONFIG_ANDROID
	.onebox_bdroid_init = rsi_bdroid_init,
	.onebox_bdroid_deinit = rsi_bdroid_deinit
#endif
};

inline struct onebox_bt_osd_operations 
*onebox_get_bt_osd_operations_from_origin()
{
  return &bt_osd_ops;
}
