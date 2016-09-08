/**
 * @file onebox_host_intf_ops.h
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
//#include "onebox_thread.h"
#include "onebox_linux.h"

#include <linux/usb.h>

/* HOST_INTERFACE OPERATIONS */
struct onebox_osd_host_intf_operations
{
	ONEBOX_STATUS (*onebox_read_register)(PONEBOX_ADAPTER adapter, uint32 Addr, uint8 *data);
	int (*onebox_write_register)(PONEBOX_ADAPTER adapter,uint8 reg_dmn,uint32 Addr,uint8 *data);
	ONEBOX_STATUS (*onebox_read_multiple)(PONEBOX_ADAPTER adapter, uint32 Addr, uint32 Count, uint8 *data );
	ONEBOX_STATUS (*onebox_write_multiple)(PONEBOX_ADAPTER adapter, uint32 Addr, uint8 *data, uint32 Count, netbuf_ctrl_block_t *netbuf_cb);
	int (*onebox_register_drv)(void);
	void (*onebox_unregister_drv)(void);
	ONEBOX_STATUS (*onebox_remove)(void);
	void (*onebox_rcv_urb_submit) (PONEBOX_ADAPTER adapter, struct urb *urb, uint8 ep_num);
	ONEBOX_STATUS (*onebox_ta_write_multiple)(PONEBOX_ADAPTER adapter, uint32 Addr, uint8 *data, uint32 Count);
	ONEBOX_STATUS (*onebox_ta_read_multiple)(PONEBOX_ADAPTER adapter, uint32 Addr, uint8 *data, uint32 Count);
	int32 (*onebox_deregister_irq)(PONEBOX_ADAPTER adapter);
	ONEBOX_STATUS (*onebox_master_reg_read)(PONEBOX_ADAPTER adapter,uint32 addr,uint32 *data, uint16 size);
	ONEBOX_STATUS (*onebox_master_reg_write)(PONEBOX_ADAPTER adapter, unsigned long addr, unsigned long data, uint16 size);
};

/* HOST_INTERFACE OPERATIONS */
struct onebox_osi_host_intf_operations
{
	ONEBOX_STATUS (*onebox_host_intf_read_pkt)(PONEBOX_ADAPTER adapter,uint8 *pkt,uint32 Len);
	//ONEBOX_STATUS (*onebox_host_intf_write_pkt)(PONEBOX_ADAPTER adapter,uint8 *pkt,uint32 Len, uint8 queueno);
	ONEBOX_STATUS (*onebox_host_intf_write_pkt)(PONEBOX_ADAPTER adapter,uint8 *pkt,uint32 Len, uint8 queueno, netbuf_ctrl_block_t *netbuf_cb);
	ONEBOX_STATUS (*onebox_ack_interrupt)(PONEBOX_ADAPTER adapter,uint8 int_BIT);
	ONEBOX_STATUS (*onebox_disable_sdio_interrupt)(PONEBOX_ADAPTER adapter);
	ONEBOX_STATUS (*onebox_init_host_interface)(PONEBOX_ADAPTER adapter);
	ONEBOX_STATUS (*onebox_master_access_msword)(PONEBOX_ADAPTER adapter, uint16 ms_word);
};
/* EOF */
