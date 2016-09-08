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
#include "wlan_common.h"
#include "onebox_linux.h"

/* osd operations can be called by Core */
static struct onebox_wlan_osd_operations wlan_osd_ops= 
{
	.onebox_init_wlan_thread=init_wlan_thread,
	.onebox_kill_wlan_thread=kill_wlan_thread,
	.onebox_get_skb_cb=get_skb_cb,
	.onebox_per_thread=per_thread,
	.onebox_kill_per_thread=kill_per_thread,
};

struct onebox_wlan_osd_operations *onebox_get_wlan_osd_operations_from_origin(void)
{
  return (&wlan_osd_ops);
}
