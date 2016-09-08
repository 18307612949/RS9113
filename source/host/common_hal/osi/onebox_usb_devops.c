/**
 *
 * @file   onebox_dev_ops.c
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
 * The file contains the initialization part of the SDBus driver and Loading of the 
 * TA firmware.
 */

/* include files */
#include "onebox_common.h"
#include "onebox_hal.h"
#include "onebox_linux.h"
#include "onebox_pktpro.h"
#include "onebox_host_intf_ops.h"
#include "onebox_sdio_intf.h"
#include "onebox_zone.h"
#include <linux/usb.h>

#if 0
ONEBOX_EXTERN uint8 process_usb_rcv_pkt(PONEBOX_ADAPTER adapter, uint32 pkt_len, uint8 pkt_type);
#endif

ONEBOX_EXTERN uint8 deploy_packet_to_assets(PONEBOX_ADAPTER adapter, netbuf_ctrl_block_t *nb_deploy);
ONEBOX_EXTERN uint8 process_unaggregated_pkt(PONEBOX_ADAPTER adapter, netbuf_ctrl_block_t *nb_deploy, int32 total_len);

ONEBOX_STATUS usb_load_data_master_write(PONEBOX_ADAPTER adapter, uint32 base_address, uint32 instructions_sz,
	uint32 block_size, uint8 *ta_firmware)
{
	uint32 num_blocks;
	//uint16 msb_address;
	uint32 cur_indx , ii;
	uint8  temp_buf[256];
	num_blocks = instructions_sz / block_size;
	//msb_address = base_address >> 16;
	ONEBOX_DEBUG(ONEBOX_ZONE_INFO,(TEXT("num_blocks: %d\n"),num_blocks));

	for (cur_indx = 0,ii = 0; ii < num_blocks; ii++,cur_indx += block_size)
	{
		adapter->os_intf_ops->onebox_memset(temp_buf, 0, block_size);
		adapter->os_intf_ops->onebox_memcpy(temp_buf, ta_firmware + cur_indx, block_size);
		if (adapter->osd_host_intf_ops->onebox_ta_write_multiple(adapter,
					base_address,
					(uint8 *)(temp_buf),
					block_size)
				!=ONEBOX_STATUS_SUCCESS)
		{
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
				(TEXT("%s: failure: %d\n"), __func__, __LINE__));


			return ONEBOX_STATUS_FAILURE;
		}      
		ONEBOX_DEBUG(ONEBOX_ZONE_INFO,
				(TEXT("%s: loading block: %d\n"), __func__,ii));
		base_address += block_size;
	}

	if (instructions_sz % block_size)
	{
		adapter->os_intf_ops->onebox_memset(temp_buf, 0, block_size);
		adapter->os_intf_ops->onebox_memcpy(temp_buf,
				ta_firmware + cur_indx,
				instructions_sz % block_size);
		if (adapter->osd_host_intf_ops->onebox_ta_write_multiple(adapter,
					base_address,
					(uint8 *)(temp_buf),
					instructions_sz % block_size
					)!=ONEBOX_STATUS_SUCCESS)
		{
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
				(TEXT("%s: failure: %d\n"), __func__, __LINE__));
			return ONEBOX_STATUS_FAILURE;
		}
		ONEBOX_DEBUG(ONEBOX_ZONE_DATA_SEND,
				(TEXT("*Written Last Block in Address 0x%x Successfully**\n"),
				 cur_indx | SD_REQUEST_MASTER));
	}
	return ONEBOX_STATUS_SUCCESS;
}

uint8 process_usb_rcv_pkt(PONEBOX_ADAPTER adapter, netbuf_ctrl_block_t *netbuf_recv_pkt, uint8 pkt_type)
{
		ONEBOX_STATUS status = ONEBOX_STATUS_SUCCESS;

	if (ZIGB_PKT == pkt_type) {
#ifndef USE_INTCONTEXT
		adapter->os_intf_ops->onebox_netbuf_queue_tail(&adapter->deferred_rx_queue,
				netbuf_recv_pkt->pkt_addr);
		adapter->os_intf_ops->onebox_queue_work(adapter->int_work_queue, 
				&adapter->defer_work);
#else
		status = deploy_packet_to_assets(adapter, netbuf_recv_pkt);
		if (status) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("FAILED TO DEPLOY part of aggr packet[%p]\n"), netbuf_recv_pkt));
		}
#endif			
	}	else {
		/* Handling unaggregated packets(As No aggregation in USB), in BT/WLAN protocols */
		status = process_unaggregated_pkt(adapter, netbuf_recv_pkt, netbuf_recv_pkt->len);
	}

	FUNCTION_EXIT(ONEBOX_ZONE_INFO);
	return status;
}
EXPORT_SYMBOL(process_usb_rcv_pkt);
