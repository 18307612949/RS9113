/**
 * @file onebox_usb_main_osi.c
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
 * The file contains Generic HAL changes for USB.
 */

#include "onebox_common.h"
#include "onebox_sdio_intf.h"
#include "onebox_host_intf_ops.h"
#include "onebox_intf_ops.h"
#include "onebox_zone.h"

/**
 * This function writes the packet to the device.
 * @param  Pointer to the driver's private data structure
 * @param  Pointer to the data to be written on to the device
 * @param  Length of the data to be written on to the device.  
 * @return 0 if success else a negative number. 
 */
ONEBOX_STATUS usb_host_intf_write_pkt(PONEBOX_ADAPTER adapter, uint8 *pkt, uint32 Len, uint8 q_no, netbuf_ctrl_block_t *netbuf_cb)
{
	uint32 Address;
	uint32 queueno = (q_no & 0x7);
	ONEBOX_STATUS status = ONEBOX_STATUS_SUCCESS;

	if( !Len  && (queueno == ONEBOX_WIFI_DATA_Q ))
	{
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,(TEXT(" Wrong length \n")));
		return ONEBOX_STATUS_FAILURE;
	} /* End if <condition> */  

	/* Fill endpoint numbers based on queueno */
	if ((queueno == COEX_TX_Q) || (queueno == WLAN_TX_M_Q) || (queueno == WLAN_TX_D_Q)) {
		Address = 1;
	//} else if ((queueno == BT_TX_Q) || (queueno == ZIGB_TX_Q)) {
	} else {
		Address = 2;	
	}

	status = adapter->osd_host_intf_ops->onebox_write_multiple(adapter,
	                                                           Address, 
	                                                           (uint8 *)pkt,
	                                                           Len,
                                                             netbuf_cb);
	if (status != ONEBOX_STATUS_SUCCESS)
	{
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
		             (TEXT("%s: Unable to write onto the card: %d\n"),__func__,
		             status));
		ONEBOX_DEBUG( ONEBOX_ZONE_DEBUG, (" <======= Desc details written previously ========== >\n"));
		adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_DEBUG, (PUCHAR)adapter->prev_desc, FRAME_DESC_SZ);
		ONEBOX_DEBUG( ONEBOX_ZONE_DEBUG, (" Current Pkt: Card write PKT details\n"));
		adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_DEBUG, (PUCHAR)pkt, Len);
	} /* End if <condition> */

	ONEBOX_DEBUG(ONEBOX_ZONE_DATA_SEND,
	             (TEXT("%s:Successfully written onto card\n"),__func__));
	return status;
}/*End <usb_host_intf_write_pkt>*/

/**
 * This function reads the packet from the SD card.
 * @param  Pointer to the driver's private data structure
 * @param  Pointer to the packet data  read from the the device
 * @param  Length of the data to be read from the device.  
 * @return 0 if success else a negative number. 
 */
ONEBOX_STATUS usb_host_intf_read_pkt(PONEBOX_ADAPTER adapter,uint8 *pkt,uint32 length)
{
	//uint32 Blocksize = adapter->ReceiveBlockSize;
	ONEBOX_STATUS Status  = ONEBOX_STATUS_SUCCESS;
	//uint32 num_blocks;

	ONEBOX_DEBUG(ONEBOX_ZONE_DATA_RCV,(TEXT( "%s: Reading %d bytes from the card\n"),__func__, length));
	if (!length)
	{
		//ONEBOX_DEBUG(ONEBOX_ZONE_DEBUG,
		 //            (TEXT( "%s: Pkt size is zero\n"),__func__));
		return Status;
	}

	//num_blocks = (length / Blocksize);
	
	/*Reading the actual data*/  
	Status = adapter->osd_host_intf_ops->onebox_read_multiple(adapter,
	                                                          length,
	                                                          length, /*num of bytes*/
	                                                          (uint8 *)pkt); 

	if (Status != ONEBOX_STATUS_SUCCESS)
	{
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,(TEXT(
		             "%s: Failed to read frame from the card: %d\n"),__func__,
		             Status));
		ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("Card Read PKT details len =%d :\n", length));
		adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_ERROR, (PUCHAR)pkt, length);

		return Status;
	}
	return Status;
}



