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

#include "onebox_common.h"
#include "onebox_linux.h"
#include "onebox_host_intf_ops.h"
#include "onebox_intf_ops.h"
#include "onebox_sdio_intf.h"

static struct onebox_osi_host_intf_operations sdio_osi_host_intf_ops =
{
	.onebox_host_intf_read_pkt = sdio_host_intf_read_pkt,
	.onebox_host_intf_write_pkt = sdio_host_intf_write_pkt,
	.onebox_ack_interrupt = sdio_ack_interrupt,
	.onebox_disable_sdio_interrupt=disable_sdio_interrupt,
	.onebox_init_host_interface=init_sdio_host_interface,
	.onebox_master_access_msword=onebox_sdio_master_access_msword,
};

struct onebox_osi_host_intf_operations *onebox_get_sdio_osi_host_intf_operations(void)
{
	return &sdio_osi_host_intf_ops;
} 
EXPORT_SYMBOL(onebox_get_sdio_osi_host_intf_operations);
