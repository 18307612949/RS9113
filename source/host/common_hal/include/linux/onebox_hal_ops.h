
/**
 * @file onebox_hal_ops.h
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
 * This file contians the function prototypes of the callbacks used across
 * differnet layers in the driver
 * 
 */
#include "onebox_common.h"
#include "onebox_thread.h"
#include "onebox_linux.h"
#include "onebox_hal.h"

#include <linux/usb.h>

/* COEX OPERATIONS */
struct onebox_coex_osi_operations
{
	void (*onebox_handle_interrupt)(PONEBOX_ADAPTER adapter);
	void (*onebox_coex_pkt_processor)(PONEBOX_ADAPTER adapter);
	void (*onebox_dump)(int32 dbg_zone_l, PUCHAR msg_to_print_p,int32 len);
	ONEBOX_STATUS (*onebox_device_init)(PONEBOX_ADAPTER adapter, uint8 fw_load);
	ONEBOX_STATUS (*onebox_device_deinit)(PONEBOX_ADAPTER adapter);
	void (*onebox_coex_sched_pkt_xmit)(PONEBOX_ADAPTER adapter);
	void (*onebox_coex_transmit_thread)(void *data);
	ONEBOX_STATUS (*onebox_coex_mgmt_frame)(PONEBOX_ADAPTER adapter, uint16 *addr, uint16 len);
	ONEBOX_STATUS (*onebox_manual_flash_write)(PONEBOX_ADAPTER adapter);
	ONEBOX_STATUS (*onebox_auto_flash_write)(PONEBOX_ADAPTER adapter, uint8 *, uint32);
	ONEBOX_STATUS (*onebox_flash_read)(PONEBOX_ADAPTER adapter);
	ONEBOX_STATUS (*onebox_auto_fw_upgrade)(PONEBOX_ADAPTER adapter, uint8 *flash_content, uint32 content_size);
 	int (*onebox_bl_write_cmd)(PONEBOX_ADAPTER adapter, uint8 cmd, uint8 exp_resp, uint16 *cmd_resp);
	ONEBOX_STATUS (*onebox_read_reg_params) (PONEBOX_ADAPTER adapter);
	ONEBOX_STATUS (*onebox_common_hal_init)(struct driver_assets *d_assets, PONEBOX_ADAPTER adapter);
	void (*onebox_common_hal_deinit)(struct driver_assets *d_assets, PONEBOX_ADAPTER adapter);
};

/* EOF */
