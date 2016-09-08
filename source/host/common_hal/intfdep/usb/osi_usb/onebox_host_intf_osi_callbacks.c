/**
 * @file onebox_host_intf_osi_callbacks.c
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
 * This file contains callbacks for all operations defined in host_interface 
 * specific files
 */

#include "onebox_host_intf_ops.h"
#include "onebox_linux.h"
#include "onebox_sdio_intf.h"
#include "onebox_intf_ops.h"

static struct onebox_osi_host_intf_operations usb_osi_host_intf_ops =
{
	.onebox_host_intf_read_pkt = usb_host_intf_read_pkt,
	.onebox_host_intf_write_pkt = usb_host_intf_write_pkt,
};

struct onebox_osi_host_intf_operations *onebox_get_usb_osi_host_intf_operations(void)
{
	return &usb_osi_host_intf_ops;
} 
EXPORT_SYMBOL(onebox_get_usb_osi_host_intf_operations);
