/**
* @file onebox_devdep_mgmt.c
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
* The file contains the message information exchanged between the 
* driver and underlying device/
*/

#include "onebox_common.h"
#include "onebox_host_intf_ops.h"
#include "onebox_intf_ops.h"
#include "onebox_sdio_intf.h"
#include "onebox_zone.h"
#include "wlan_common.h"

#define MAX_SNIFFER_MODE_PKT_LEN  4096
//extern ONEBOX_STATUS wlan_module_init(struct driver_assets *d_assets);

static int inaugurate_assets(struct driver_assets *d_assets, int32 asset_id)
{
	int rc = ONEBOX_STATUS_FAILURE;
	switch (asset_id) {
	case WLAN_ID:
#if 0
					/* WLAN */
					if ((d_assets->techs[WLAN_ID].drv_state == MODULE_INSERTED) && (d_assets->techs[WLAN_ID].drv_state != MODULE_ACTIVE)) {
									if (d_assets->techs[WLAN_ID].inaugurate) {
													rc = d_assets->techs[WLAN_ID].inaugurate(d_assets);
													if (rc) {
																	ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("hal.%s: failed to initialise"
																													"wlan layer\n, moving layer to dormant", __func__));
													}
									}
					}
#endif
#ifdef WLAN_ENABLE
					rc = wlan_module_init(d_assets);
#endif

		break;
	case BT_ID:
		/* BLUETOOTH */
#if 0
		if (d_assets->techs[BT_ID].drv_state == MODULE_INSERTED) {
			if (d_assets->techs[BT_ID].inaugurate) {
				rc = d_assets->techs[BT_ID].inaugurate(d_assets);
				if (rc) {
					ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("hal.%s: failed to initialise"
						"bt layer, moving layer to dormant\n", __func__));
				}
			}
		}
#endif
#ifdef BT_ENABLE
		rc = bt_module_init(d_assets);
#endif
		break;
	case ZB_ID:
		/* ZIGBEE */
#if 0
		if (d_assets->techs[ZB_ID].drv_state == MODULE_INSERTED) {
			if (d_assets->techs[ZB_ID].inaugurate) {
				rc = d_assets->techs[ZB_ID].inaugurate(d_assets);
				if (rc) {
					ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("hal.%s: failed to initialise"
						"zigb layer, moving layer to dormant\n", __func__));
				}
			}
		}
#endif
#ifdef ZIGB_ENABLE
		rc = zb_module_init(d_assets);
#endif
		break;
	default:
		break;
	}

	return rc;
}

static inline  
ONEBOX_STATUS process_ulp_sleep_notify(PONEBOX_ADAPTER adapter, uint8 *msg)
{
		struct driver_assets *d_assets = adapter->d_assets;
		uint8 sleep_status = 0;
#ifdef USE_SDIO_INTF
		struct mmc_host *host;
#endif

		sleep_status = (msg[12] & BIT(0));
		if(sleep_status == ULP_SLEEP_ENTRY) {
				adapter->ulp_sleep_token = (*(uint16 *)&msg[12] >> 4);
				ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("%s Line %d Recvd ULP Sleep Request token %d\n"), 
										__func__, __LINE__,adapter->ulp_sleep_token));
				sleep_entry_recvd(adapter);
		} else {
				ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Recvd Sleep exit token %d\n"), 
										adapter->ulp_sleep_token));
				adapter->ulp_sleep_token = 0xABCD;

#ifdef USE_SDIO_INTF
				if (adapter->host_intf_type == HOST_INTF_SDIO) {
					host = adapter->sdio_pfunction->card->host;
					host->ops->set_ios(host, &host->ios);
				}
#endif

				sleep_exit_recvd(adapter);
		}
		ONEBOX_DEBUG( ONEBOX_ZONE_INFO, ("In %s Line %d cmn tx %d\n", __func__, __LINE__, d_assets->common_hal_tx_access));
		return ONEBOX_STATUS_SUCCESS;

}

static void onebox_update_common_hal_tx_access(struct driver_assets *d_assets, uint8 proto_id) 
{
	//struct driver_assets *d_assets = onebox_get_driver_asset();
	PONEBOX_ADAPTER adapter = (PONEBOX_ADAPTER)d_assets->global_priv;

	ONEBOX_DEBUG( ONEBOX_ZONE_INFO, ("In %s Line %d common has fsm is %d\n", __func__, __LINE__, d_assets->common_hal_fsm));
	switch(d_assets->common_hal_fsm)
	{
		case COMMAN_HAL_WAIT_FOR_CARD_READY:
		/*Check whether ALL protocols Card readys are received*/
      if(d_assets->common_hal_fw_state == FW_ACTIVE)
					d_assets->common_hal_fsm  = COMMON_HAL_SEND_CONFIG_PARAMS;

			if(d_assets->common_hal_fsm != COMMON_HAL_SEND_CONFIG_PARAMS)
				break;

		case COMMON_HAL_SEND_CONFIG_PARAMS:
			/*Send Each protocol Sleep default frame*/
      ONEBOX_DEBUG( ONEBOX_ZONE_INFO, ("Sending common dev config params \n"));
				if (onebox_configure_common_dev_params(adapter) == ONEBOX_STATUS_SUCCESS) {
					ONEBOX_DEBUG( ONEBOX_ZONE_INFO, ("SUCCESS Sending common dev config params \n"));
				} else {
					ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("FAILURE Sending common dev config params \n"));
				}
			d_assets->common_hal_fsm  = COMMON_HAL_TX_ACCESS;

		break;

    case COMMON_HAL_TX_ACCESS:
    d_assets->common_hal_tx_access = true;
#ifdef GPIO_HANDSHAKE
			if(d_assets->lp_ps_handshake_mode == GPIO_HAND_SHAKE || d_assets->ulp_ps_handshake_mode == GPIO_HAND_SHAKE){
			gpio_host_tx_intention(adapter,GPIO_HIGH);
			}
#endif
			ONEBOX_DEBUG( ONEBOX_ZONE_INFO, ("In %s %d\n", __func__, __LINE__));
		
		/*At this point we recvd all protocol's card ready and give tx_access 
 		 * to protocols who are already waiting for TX_access. SO we dont 
 		 * require proto_id argument here, hence reseting and checking
 		 * for different protocols.
		*/ 

			if(d_assets->techs[proto_id].fw_state == FW_ACTIVE) 
			{
							if (d_assets->techs[proto_id].deregister_flags 
															&& (d_assets->techs[proto_id].drv_state == MODULE_ACTIVE)) {
											d_assets->techs[proto_id].deregister_flags = 0;
											ONEBOX_DEBUG( ONEBOX_ZONE_INFO, ("Waking up an event  In %s Line %d proto is %d\n", __func__, __LINE__, proto_id));
											d_assets->techs[proto_id].tx_access = 1;
											wake_up(&d_assets->techs[proto_id].deregister_event);
							}
			}	

    break;
		default:
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("In %s Line %d in valid state %d\n", __func__, __LINE__,d_assets->common_hal_fsm));
			break;
	}

}

/**
 * This function processes the hal management packets
 *
 * @param 
 *  adapter  Pointer to driver private structure
 * @param 
 *  msg      Received packet
 * @param 
 *  len      Length of the received packet
 *
 * @returns 
 *  ONEBOX_STATUS_SUCCESS on success, or corresponding negative
 *  error code on failure
 */
int32 onebox_process_coex_rx_pkt(PONEBOX_ADAPTER adapter, uint8 *msg)
{
	uint16 msg_type;
	uint32  msg_len;
	uint8  sub_type;
	struct driver_assets *d_assets = adapter->d_assets;

	ONEBOX_STATUS status = ONEBOX_STATUS_FAILURE;
	
	msg_len   = (*(uint16 *)&msg[0] & 0x0fff);
	
	/* Type is upper 5bits of descriptor */
	msg_type = msg[2];
	sub_type = (msg[15] & 0xff);

	adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_MGMT_DUMP, msg, FRAME_DESC_SZ);
	switch (adapter->fsm_state) {
	case FSM_CARD_NOT_READY: 
		if ((msg_type == CARD_READY_IND && adapter->Driver_Mode == QSPI_FLASHING) 
				|| ((msg_type == RX_MISC_IND) && (sub_type == FW_UPGRADE_REQ)))
		{
			ONEBOX_DEBUG(ONEBOX_ZONE_INFO,(TEXT ("TRIGGER FLASHING RECEIVED FROM COEX FIRMWARE\n"))); 
			adapter->flashing_mode_on = 1;
				if(onebox_qspi_bootup_params(adapter) == ONEBOX_STATUS_SUCCESS) {
					ONEBOX_DEBUG(ONEBOX_ZONE_FSM,
						     (TEXT("%s: BOOTUP Parameters loaded successfully\n"),__func__));
					adapter->fsm_state = FSM_LOAD_BOOTUP_PARAMS ;
				} else {
					ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,(TEXT ("%s: Failed to load bootup parameters\n"), 
																						__func__));
				}
				
		} 
		status = ONEBOX_STATUS_SUCCESS;
	break;
	case FSM_FLASH_BURN:
	{
		ONEBOX_DEBUG(ONEBOX_ZONE_INFO,(TEXT( "FLASH_BURN : \n")));
		if(sub_type == EEPROM_WRITE)
		{
			ONEBOX_DEBUG(ONEBOX_ZONE_INFO,(TEXT(
							"FLASH_WRITE: confirm Received:  \n")));
			adapter->eeprom_cfm_count++;
			adapter->os_intf_ops->onebox_set_event(&(adapter->flash_event));
		}
		else if(sub_type == EEPROM_READ_TYPE)
		{
			ONEBOX_DEBUG(ONEBOX_ZONE_INFO,(TEXT(
							"FLASH_READ: confirm Received:  \n")));

			msg_len   = (*(uint16 *)&msg[0] & 0x0fff);
			if (msg_len > 0)
			{  
				ONEBOX_DEBUG( ONEBOX_ZONE_INFO, ("eeprom length: %d, received msg_len %d\n",adapter->eeprom.length, msg_len));
				if(adapter->eeprom.length != msg_len) {
					ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("Received different length from the requested flash length\n"));
					return ONEBOX_STATUS_FAILURE;
				}
				adapter->os_intf_ops->onebox_memcpy((PVOID)(&adapter->DataRcvPacket[0]), &msg[16], msg_len);
				adapter->os_intf_ops->onebox_set_event(&(adapter->flash_event));
			}  
		}
		status = ONEBOX_STATUS_SUCCESS;
	}
	break;
	case FSM_LOAD_BOOTUP_PARAMS:
	{
		if (msg_type == TA_CONFIRM_TYPE && sub_type == BOOTUP_PARAMS_REQUEST)
		{
			ONEBOX_DEBUG(ONEBOX_ZONE_INFO,
					     (TEXT("%s: Received bootup parameters confirm"
							"sucessfully\n"),__FUNCTION__));
			adapter->flashing_started = 1;
			status = ONEBOX_STATUS_SUCCESS;
			adapter->os_intf_ops->onebox_set_event(&(adapter->sdio_scheduler_thread_handle.thread_event));
		}
	}
	break;
	case FSM_DEVICE_READY:
	{
		if (msg[2] == COMMON_HAL_CARD_READY_IND) {
			ONEBOX_DEBUG(ONEBOX_ZONE_INFO,
					     (TEXT("%s: COMMON CARD READY RECVD SUCESSFULLY\n"),__FUNCTION__));
			d_assets->common_hal_fw_state = FW_ACTIVE;
			onebox_update_common_hal_tx_access(d_assets, WLAN_ID);
			status = ONEBOX_STATUS_SUCCESS;
		} else if(msg[2] == ULP_SLEEP_NOTIFY) {
			status = process_ulp_sleep_notify(adapter, msg);
		} else if(msg[2] == IAP_CONFIG) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
					(TEXT("%s Line %d: MFI INIT CONFIRM"
					      "SUCESSFULLY\n"),__func__, __LINE__));

			if (msg[15] == 0) {
				ONEBOX_DEBUG(ONEBOX_ZONE_DEBUG,
					(TEXT("IAP INIT received\n")));
			}
			else if(msg[15] == 1) {
				ONEBOX_DEBUG(ONEBOX_ZONE_DEBUG,
					(TEXT("MFI SIGN received\n")));
				d_assets->mfi_signature = kmalloc(msg_len, GFP_KERNEL);
				if (msg_len == 0) {
					adapter->os_intf_ops->onebox_set_event(&(d_assets->iap_event));
					return status;
				}
				ONEBOX_DEBUG(ONEBOX_ZONE_DEBUG,
					(TEXT("MFI SIG RECVD FROM FW msg len is %d\n"), msg_len));
				adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_ERROR, msg, msg_len);
				memcpy(d_assets->mfi_signature, msg + FRAME_DESC_SZ + 2, msg_len - 2);
		  	adapter->os_intf_ops->onebox_set_event(&(d_assets->iap_event));
			} else if (msg[15] == 2) {
				ONEBOX_DEBUG(ONEBOX_ZONE_DEBUG,
					(TEXT("MFI CERTIFICATE received\n")));
				d_assets->mfi_certificate = kmalloc(msg_len, GFP_KERNEL);
				if (msg_len == 0) {
		  			adapter->os_intf_ops->onebox_set_event(&(d_assets->iap_event));
					return status;
				}
				memcpy(d_assets->mfi_certificate, msg + FRAME_DESC_SZ + 2, msg_len - 2);
				adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_DEBUG, d_assets->mfi_certificate, msg_len);
				adapter->os_intf_ops->onebox_set_event(&(d_assets->iap_event));
			}
			status = ONEBOX_STATUS_SUCCESS;
		} else {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
					     (TEXT("%s Line %d: INVALID COEX PKT TYPE RECVD\n")
							 ,__func__, __LINE__));
		}
	}
	break;
	default:
				ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
		(TEXT("COMMON HAL FSM state is not handled state is %d\n"), adapter->fsm_state));
		break;
	} /* End switch (adapter->fsm_state) */

	return status;
}

int32 panic_stop_all_activites(PONEBOX_ADAPTER adapter, uint32 reg_to_ack)
{
	struct driver_assets *d_assets = adapter->d_assets;
	
	if (adapter->host_intf_type == HOST_INTF_SDIO) {
		int32 status;
		status = adapter->osi_host_intf_ops->onebox_ack_interrupt(adapter, 
				(1 << reg_to_ack));
		if (status != ONEBOX_STATUS_SUCCESS) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s: Unable to ACK Interrupt\n", __func__));
		}
	}

	adapter->fsm_state = FSM_CARD_NOT_READY;

	adapter->stop_card_write = 2;

	d_assets->card_state = GS_CARD_DETACH;
	d_assets->techs[WLAN_ID].fw_state = FW_INACTIVE; 
	d_assets->techs[BT_ID].fw_state   = FW_INACTIVE; 
	d_assets->techs[ZB_ID].fw_state   = FW_INACTIVE; 

	return ONEBOX_STATUS_FAILURE;
}

uint8 process_unaggregated_pkt(PONEBOX_ADAPTER adapter, netbuf_ctrl_block_t *nb_deploy, int32 total_len)
{
	uint8 *frame_desc_addr  = NULL;
	uint8 extended_desc     = 0;

	uint16 actual_length    = 0;
	uint16 offset           = 0;

	uint32 payload_len      = 0;
	uint32 length           = 0;
	uint32 united_len	= 0;
	//uint32 pkt_len           = nb_deploy->len ;

#ifdef USE_INTCONTEXT
	ONEBOX_STATUS status = ONEBOX_STATUS_SUCCESS;
#endif
	do {
		frame_desc_addr = &nb_deploy->data[0];

		actual_length   = *(uint16 *)&frame_desc_addr[0];
		offset          = *(uint16 *)&frame_desc_addr[2];
#if 0
		ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s:actual_len = %d offset = %d pkt_len = %d\n "
				"total_len = %d extended_desc = %d\n", __func__, 
				actual_length, offset, length, pkt_len, extended_desc));
#endif
		// check - 3
		/*
		 * actual length should atleast
		 * length of host descritor(4) and FRAME_DESC_SZ 
		 */ 
		if ((actual_length < (4 + FRAME_DESC_SZ)) ||
				(offset < 4)) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("%s:actual_length(%d) is less than 20 or"
							"offset(%d) is less than 4\n"),
						__func__, actual_length, offset));
			goto pkt_corrupted;
		}

		// check - 2
		/*
		 * actual_length should be equal to total length 
		 * (for single packet or last packet in aggregation loop.)
		 * or (actual_length + host desc + FRAME_DESC_SZ) <= total length 
		 * when one more packet is expected in the loop.
		 */
		if (!((actual_length == total_len) ||
					((actual_length + 4 + FRAME_DESC_SZ) <= total_len))) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
					(TEXT("%s:actual_length(%d) in the received packet is corrupted\n"),
					 __func__, actual_length));
			goto pkt_corrupted;
		}


		length          = (*(uint16 *)&frame_desc_addr[offset] & 0x0fff); /* coex */
		if(nb_deploy->rx_pkt_type == WLAN_PKT)
		{
			extended_desc   = (*(uint8 *)&frame_desc_addr[offset + 4] & 0x00ff); /* coex */
		}

		/*
		 *  NEW FRAME FORMAT: 
		 *  =================
		 *  pkt1[0,1]    = total host transfer lenght(2bytes size).
		 *		 = (host descriptor offset + FRAME_DESC_SZ +
		 *		    len in host descriptor(12bits) + padding)
		 *
		 *  pkt1[2,3]    = offset of host desc from start of the
		 *  	           frame + 2Bytes().
		 *
		 *  ptk1[offset] = 11:0  length(extended desc len + payload len).
		 *                 14:12 pkt info(0 - cmd pkt dies in firmware)
		 *       		          1 - on-air mgmt pkt,
		 *       		          2 - datapkt), 15 reserved
		 *
		 *  pkt1[offset+ 4] = extended descriptor length.
		 *  ptk1[offset+14] = 0:7 mgmt type (if ptk1(14:12)== 0/1)
		 *  ptk1[offset+15] = 0:7 reserved
		 *
		 *  pkt1[offset+FRAME_DESC_SZ] = start of Extended Descriptor
		 *
		 *  pkt1[offset+FRAME_DESC_SZ+Extended descriptor size] =
		 *  			payload.
		 *
		 *  old format
		 *  0 - {15:8 type, 7:0 len}
		 *    - {15 tcp checksum, 14 - ip sec, 13:12 - reserved, 11:0 - len}
		 *  7 - {2:0 - type, 10:8 - no of aggregated pkts, 15 - aggregated}  
		 */

		// check - 4
		if (adapter->host_intf_type != HOST_INTF_SDIO)
			united_len = ((offset + length + FRAME_DESC_SZ + 3) & ~3);
		else       //#elif defined(USE_SDIO_INTF)
			united_len = ((offset + length + FRAME_DESC_SZ + 255) & ~255);

		if (adapter->host_intf_type != HOST_INTF_USB) {
			if (actual_length != united_len)
			{
				ONEBOX_DEBUG (ONEBOX_ZONE_ERROR,
						(TEXT
						 ("%s:actual_length(%d) didn't match with united_len(%d)\n"),
						 __func__, actual_length, united_len));;
				goto pkt_corrupted;
			}
		}

		if (adapter->Driver_Mode != QSPI_FLASHING) {
			/*
			 * extended frame descritor should no exceed
			 * length in frame descriptor
			 */ 
			if (extended_desc > length) {
				ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,(TEXT("%s: extended_desc(%d) is more than frame_len(%d)\n"),
							__func__, extended_desc, length));
				goto pkt_corrupted;
			}

			if (adapter->Driver_Mode == WIFI_MODE_ON && !adapter->flashing_mode_on) {
				if (length > COMMON_PKT_LENGTH) {
					ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
							(TEXT("%s: Toooo big packet(%d) in Wifi Mode\n"), __func__,length));     
					goto pkt_corrupted;
				}
			} else {        //if (adapter->Driver_Mode == SNIFFER_MODE) {
				/* For Sniffer Mode, PER Mode and Other Modes */
				if (length > MAX_SNIFFER_MODE_PKT_LEN) {
					ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
							(TEXT("%s: Toooo big packet(%d) in Sniffer Mode\n"), __func__,length));     
					goto pkt_corrupted;
				}
			}
		}

		payload_len = length + FRAME_DESC_SZ;

		adapter->os_intf_ops->onebox_netbuf_adj(nb_deploy, offset);
		adapter->os_intf_ops->onebox_netbuf_trim(nb_deploy, payload_len);
		if (adapter->host_intf_type == HOST_INTF_USB) {
			if(nb_deploy->rx_pkt_type == WLAN_PKT &&
					nb_deploy->data[2] == RX_MISC_IND && 
					nb_deploy->data[15] == FW_UPGRADE_REQ) {
				nb_deploy->rx_pkt_type = COEX_PKT;
			}
		}

#ifndef USE_INTCONTEXT
		if (adapter->os_intf_ops->onebox_netbuf_queue_len(&adapter->deferred_rx_queue) <= 1000) {
			adapter->os_intf_ops->onebox_netbuf_queue_tail(&adapter->deferred_rx_queue,
					nb_deploy->pkt_addr);
		} else {
			/* Dropping has been added due to memory is getting occupied 
			 * on low embedded platforms when there is an heavy RX traffic
			 * from firmware.
			 */
			adapter->rx_packtes_dropped++;
			adapter->os_intf_ops->onebox_free_pkt(nb_deploy, 0);	
		}
#ifdef USE_WORKQUEUES
		adapter->os_intf_ops->onebox_queue_work(adapter->int_work_queue, 
				&adapter->defer_work);
#else
		adapter->os_intf_ops->onebox_tasklet_sched(&adapter->int_bh_tasklet);
#endif
#else
		status = deploy_packet_to_assets(adapter, nb_deploy);
		if (status) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("FAILED TO DEPLOY part of aggr packet[%p]\n"), nb_deploy));
			return ONEBOX_STATUS_FAILURE;
		}
#endif

	}while (0);

	FUNCTION_EXIT(ONEBOX_ZONE_INFO);
	return ONEBOX_STATUS_SUCCESS;
pkt_corrupted:
	ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("Corrupted Pkt len is %d total len %d\n", nb_deploy->len, total_len));
	adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_ERROR, nb_deploy->data, nb_deploy->len);
	adapter->os_intf_ops->onebox_free_pkt(nb_deploy, 0);	
	return panic_stop_all_activites(adapter, SD_MSDU_PACKET_PENDING);
}

static uint8 process_lmac_aggregated_pkt(PONEBOX_ADAPTER adapter, uint32 pkt_len, uint8 pkt_type)
{
	uint8 queueno           = 0;
	uint8 extended_desc     = 0;
	uint8 *frame_desc_addr  = NULL;

	uint16 actual_length    = 0;
	uint16 offset           = 0;

	uint32 payload_len      = 0;
	uint32 index;
	uint32 length          = 0;
	uint32 united_len      = 0;
#ifdef USE_INTCONTEXT
	ONEBOX_STATUS status = ONEBOX_STATUS_SUCCESS;
#endif

	netbuf_ctrl_block_t *nb_deploy = NULL;

	index = 0;
	do {
		frame_desc_addr = &adapter->DataRcvPacket[index];

		actual_length   = *(uint16 *)&frame_desc_addr[0];
		offset          = *(uint16 *)&frame_desc_addr[2];
#if 0
		ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s:actual_len = %d offset = %d pkt_len = %d\n "
			"total_len = %d extended_desc = %d\n", __func__, 
			actual_length, offset, length, pkt_len, extended_desc));
#endif

// check - 3
		/*
		 * actual length should atleast
		 * length of host descritor(4) and FRAME_DESC_SZ 
		 */ 
		if ((actual_length < (4 + FRAME_DESC_SZ)) ||
		    (offset < 4)) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("%s:actual_length(%d) is less than 20 or"
				"offset(%d) is less than 4\n"),
				__func__, actual_length, offset));
			return panic_stop_all_activites(adapter, SD_MSDU_PACKET_PENDING);
		}

// check - 2
		/*
		 * actual_length should be equal to total length 
		 * (for single packet or last packet in aggregation loop.)
		 * or (actual_length + host desc + FRAME_DESC_SZ) <= total length 
		 * when one more packet is expected in the loop.
		 */
		if (!((actual_length == pkt_len) ||
		    ((actual_length + 4 + FRAME_DESC_SZ) <= pkt_len))) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
				(TEXT("%s: LIne %dactual_length(%d) in the received packet is corrupted\n"),
				__func__, __LINE__, actual_length));
			
			return panic_stop_all_activites(adapter, SD_MSDU_PACKET_PENDING);
		}

		length          = (*(uint16 *)&frame_desc_addr[offset] & 0x0fff); /* coex */
		
		/*Make it protocol independent */
		if(pkt_type == WLAN_PKT)
			extended_desc   = (*(uint8 *)&frame_desc_addr[offset + 4] & 0x00ff);

		queueno         = (uint32)((*(uint16 *)&frame_desc_addr[offset] & 0x7000) >> 12);
		
		/*
		 *  NEW FRAME FORMAT: 
		 *  =================
		 *  pkt1[0,1]    = total host transfer lenght(2bytes size).
		 *		 = (host descriptor offset + FRAME_DESC_SZ +
		 *		    len in host descriptor(12bits) + padding)
		 *
		 *  pkt1[2,3]    = offset of host desc from start of the
		 *  	           frame + 2Bytes().
		 *
		 *  ptk1[offset] = 11:0  length(extended desc len + payload len).
		 *                 14:12 pkt info(0 - cmd pkt dies in firmware)
		 *       		          1 - on-air mgmt pkt,
		 *       		          2 - datapkt), 15 reserved
		 *
		 *  pkt1[offset+ 4] = extended descriptor length.
		 *  ptk1[offset+14] = 0:7 mgmt type (if ptk1(14:12)== 0/1)
		 *  ptk1[offset+15] = 0:7 reserved
		 *
		 *  pkt1[offset+FRAME_DESC_SZ] = start of Extended Descriptor
		 *
		 *  pkt1[offset+FRAME_DESC_SZ+Extended descriptor size] =
		 *  			payload.
		 *
		 *  old format
		 *  0 - {15:8 type, 7:0 len}
		 *    - {15 tcp checksum, 14 - ip sec, 13:12 - reserved, 11:0 - len}
		 *  7 - {2:0 - type, 10:8 - no of aggregated pkts, 15 - aggregated}  
		 */
// check - 4
		if (adapter->host_intf_type != HOST_INTF_SDIO) 
			united_len = ((offset + length + FRAME_DESC_SZ + 3) & ~3);
		else     //#elif defined(USE_SDIO_INTF)
			united_len = ((offset + length + FRAME_DESC_SZ + 255) & ~255);

		if (actual_length != united_len) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,(TEXT("%s:Line %dactual_length(%d) didn't match with united_len(%d)\n"), 
				__func__, __LINE__, actual_length, united_len));;
			return panic_stop_all_activites(adapter, SD_MSDU_PACKET_PENDING);
		}

// check - 5
		/*
		 * extended frame descritor should no exceed
		 * length in frame descriptor
		 */ 
		if (extended_desc > length) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,(TEXT("%s: extended_desc(%d) is more than frame_len(%d)\n"),
				__func__, extended_desc, length));
			return panic_stop_all_activites(adapter, SD_MSDU_PACKET_PENDING);
		}

		if (adapter->Driver_Mode == WIFI_MODE_ON && !adapter->flashing_mode_on) {
			if (length > COMMON_PKT_LENGTH) {
				ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
					  (TEXT("%s: Toooo big packet(%d) in Wifi Mode\n"), __func__,length));     
				return panic_stop_all_activites(adapter, SD_MSDU_PACKET_PENDING);
			}
		} else {        //if (adapter->Driver_Mode == SNIFFER_MODE) {
			/* For Sniffer Mode, PER Mode and Other Modes */
			if (length > MAX_SNIFFER_MODE_PKT_LEN) {
				ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
					  (TEXT("%s: Toooo big packet(%d) in Sniffer Mode\n"), __func__,length));     
				return panic_stop_all_activites(adapter, SD_MSDU_PACKET_PENDING);
			}
		}
		
		//payload_len = length + extended_desc + FRAME_DESC_SZ;
		payload_len = length + FRAME_DESC_SZ;

		nb_deploy = adapter->os_intf_ops->onebox_alloc_skb(payload_len);
		if (!nb_deploy) {	
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Unable to allocate NETBUFCB\n")));
			return -ENOMEM;
		} 

		/*nb_deploy->dev  = adapter->dev;*/
		nb_deploy->len  = payload_len;
		nb_deploy->rx_pkt_type = pkt_type;

		adapter->os_intf_ops->onebox_add_data_to_skb(nb_deploy, payload_len);

		adapter->os_intf_ops->onebox_memcpy(nb_deploy->data,
						    frame_desc_addr + offset,
						    nb_deploy->len);
#ifndef USE_INTCONTEXT
		adapter->os_intf_ops->onebox_netbuf_queue_tail(&adapter->deferred_rx_queue,
							       nb_deploy->pkt_addr);
#else
		status = deploy_packet_to_assets(adapter, nb_deploy);
		if (status) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("FAILED TO DEPLOY part of aggr packet[%p]\n"), nb_deploy));
			return ONEBOX_STATUS_FAILURE;
		}
#endif
		index  += actual_length;
		pkt_len -= actual_length;
	} while (pkt_len > 0);
#ifndef USE_INTCONTEXT
#ifdef USE_WORKQUEUES
	adapter->os_intf_ops->onebox_queue_work(adapter->int_work_queue, 
						&adapter->defer_work);
#endif
#ifdef USE_TASKLETS
	adapter->os_intf_ops->onebox_tasklet_sched(&adapter->int_bh_tasklet);
#endif
#endif
	FUNCTION_EXIT(ONEBOX_ZONE_INFO);

	return ONEBOX_STATUS_SUCCESS;
}


/* This function handles the data/ mgmt pkts, thus indicating the packet to 
 * the corresponding protocol handler
 * @param pointer to the adapter structure
 * @param pointer to netbuf control block structure
 * @return ONEBOX_STATUS_SUCCESS on success else ONEBOX_STATUS_FAILURE
 */
uint8 deploy_packet_to_assets(PONEBOX_ADAPTER adapter, 
			      netbuf_ctrl_block_t *nb_deploy)
{
	uint8 proto             = 0;
	ONEBOX_STATUS status    = ONEBOX_STATUS_SUCCESS;
	struct driver_assets *d_assets = adapter->d_assets;

	if (nb_deploy == NULL) {	
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT(" %s:Invalid NETBUFCB\n"), __func__));
		return ONEBOX_STATUS_FAILURE;
	} 
	
	if (d_assets->card_state == GS_CARD_DETACH) {
		ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT(" %s:card is being removed dropping packets\n"), __func__));
		goto fail;
	} 

	proto = nb_deploy->rx_pkt_type;

	if ((nb_deploy->data[1] >> 4) == COEX_PKT) {
		proto = COEX_PKT;
	}

	switch (proto) {
	case COEX_PKT:
				ONEBOX_DEBUG( ONEBOX_ZONE_DEBUG, ("COEX PKT IS\n"));
				adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_DEBUG, nb_deploy->data, nb_deploy->len);
		status = onebox_process_coex_rx_pkt(adapter, nb_deploy->data);
		if (status != ONEBOX_STATUS_SUCCESS) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("RX: COEX Pkt processing failed\n"));
		}
		goto fail;
		break;
	case BT_PKT:
		if ((d_assets->techs[BT_ID].fw_state == FW_INACTIVE ) || (d_assets->bt_deregister_sent)) {
			if (nb_deploy->data[14] == BT_CARD_READY_IND) {
				ONEBOX_DEBUG( ONEBOX_ZONE_INIT, ("CARD READY RECEIVED FROM BT FIRMWARE\n"));
				adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_INFO, nb_deploy->data, nb_deploy->len);
				d_assets->techs[BT_ID].fw_state = FW_ACTIVE;
				d_assets->bt_deregister_sent = 0;
				/*
				 * Return type has to handled here
				 */
				onebox_update_common_hal_tx_access(d_assets, BT_ID);
				status = inaugurate_assets(d_assets, BT_ID);
				if (status) {
					ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("COMMON: BT modules not yet loaded\n"));
				}
			} else {
				ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("COMMON: Wating for BT CARD READY..Dropping till then\n"));
			}
			goto fail;
		} else {
			if (d_assets->bt_deregister_sent) {
				goto fail;
			
			}
			if (d_assets->techs[BT_ID].drv_state == MODULE_ACTIVE) {
				if (d_assets->techs[BT_ID].onebox_get_pkt_from_coex) {
					d_assets->techs[BT_ID].onebox_get_pkt_from_coex(d_assets, nb_deploy);
				}
			} else { 
				ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("COEX : Data packet, BT adapter is NULL\n"));
				goto fail;
			}
		}
		break;
	case WLAN_PKT:
		if (d_assets->techs[WLAN_ID].fw_state == FW_INACTIVE) {

			if (nb_deploy->data[2] == WLAN_CARD_READY_IND) {
				ONEBOX_DEBUG( ONEBOX_ZONE_INIT, ("CARD READY RECEIVED FROM WLAN FIRMWARE\n"));

				adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_ERROR, nb_deploy->data, nb_deploy->len);
				if (nb_deploy->data[FRAME_DESC_SZ] != MAGIC_WORD) {
					ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
							(TEXT("%s: Line %d UNABLE TO READ EEPROM \n"),__func__, __LINE__));
					goto fail;	
				}

				d_assets->mfg_major_verion = nb_deploy->data[18];
				d_assets->mfg_minor_verion = nb_deploy->data[19];
				if (((d_assets->mfg_major_verion > 3) && (d_assets->mfg_minor_verion > 1)) ||
						(d_assets->mfg_major_verion >= 4)) {
					d_assets->onboard_antenna = nb_deploy->data[17];
				} else {
					d_assets->onboard_antenna = d_assets->user_onboard_ant_val;
				}
				adapter->os_intf_ops->onebox_memcpy((PVOID)(&d_assets->wlan_mac_address), &nb_deploy->data[20], 6);
				d_assets->module_type = nb_deploy->data[26];
				d_assets->band_version = nb_deploy->data[27];
				ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("WLAN CARD DETAILS ARE:\n"));
				ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("MAC_ADDRESS:%02x %02x %02x %02x %02x %02x\n", 
							d_assets->wlan_mac_address[0],d_assets->wlan_mac_address[1],
							d_assets->wlan_mac_address[2],d_assets->wlan_mac_address[3],
							d_assets->wlan_mac_address[4],d_assets->wlan_mac_address[5]
							));
				ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("MFG SW Version: %d.%d\n ONBOARD_ANT %d \n Module_Type: %d \n",
							d_assets->mfg_major_verion, d_assets->mfg_minor_verion, 
							d_assets->onboard_antenna, d_assets->module_type));

				if (d_assets->band_version == 0x03) {
					ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("DUAL BAND SUPPORTED\n"));
				} else {
					ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("SINGLE BAND SUPPORTED\n"));
				}

				d_assets->techs[WLAN_ID].fw_state = FW_ACTIVE;
				onebox_update_common_hal_tx_access(d_assets, WLAN_ID);
				/*
				 * Return type has to handled here
				 */
				if (adapter->host_intf_type == HOST_INTF_USB) {
					d_assets->techs[WLAN_ID].buffer_status_reg_addr = *(uint32 *)(&nb_deploy->data[8]);
					ONEBOX_DEBUG( ONEBOX_ZONE_INFO, ("\n buffer indication address = 0x%x \n", d_assets->techs[WLAN_ID].buffer_status_reg_addr));
				}
				status = inaugurate_assets(d_assets, WLAN_ID);
				if (status) {
					ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("COMMON: WLAN module not yet loaded\n"));
				}
			} else {
				ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("COMMON: Wating for WLAN CARD READY..Dropping till then\n"));
			}
			goto fail;
		} else {
			if (d_assets->techs[WLAN_ID].drv_state == MODULE_ACTIVE) {

				if (d_assets->techs[WLAN_ID].onebox_get_pkt_from_coex) {
					d_assets->techs[WLAN_ID].onebox_get_pkt_from_coex(d_assets, nb_deploy);
				}
			} else {
			  	/* XXX: This print is causing delays in sending Deregister frame.
				 * This case arises when driver is being removed and packets are coming
				 * from firmware. Hence dropping these packets */
				//ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("COEX : Data packet, WLAN adapter is NULL\n"));
				goto fail;
			}
		}
		break;
	case ZIGB_PKT:
		if (d_assets->techs[ZB_ID].fw_state == FW_INACTIVE) {
			if (nb_deploy->data[15] == ZIGB_CARD_READY_IND) {
				ONEBOX_DEBUG( ONEBOX_ZONE_INIT, ("CARD READY RECEIVED FROM ZIGB FIRMWARE\n"));
				adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_INFO, nb_deploy->data, nb_deploy->len);
				d_assets->techs[ZB_ID].fw_state = FW_ACTIVE;
				onebox_update_common_hal_tx_access(d_assets, ZB_ID);
				/*
				 * Return type has to handled here
				 */
				status = inaugurate_assets(d_assets, ZB_ID);
				if (status) {
					ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("COMMON: ZIGB modules not yet loaded\n"));
				}
			} else {
				ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("COMMON: Wating for ZIGB CARD READY..Dropping till then\n"));
			}
			goto fail;
		} else {
			if (d_assets->techs[ZB_ID].drv_state == MODULE_ACTIVE) {
				if (d_assets->techs[ZB_ID].onebox_get_pkt_from_coex) {
					d_assets->techs[ZB_ID].onebox_get_pkt_from_coex(d_assets, nb_deploy);
				}
			}
			else { 
				ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("COEX : Data packet, ZIGB adapter is NULL\n"));
				goto fail;
			}
		}
		break;
	default:
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("%s: pkt to invalid protocol\n"), __func__)); 
		break;
	} /* End switch */      
	return ONEBOX_STATUS_SUCCESS;
fail:
	if(nb_deploy)
		adapter->os_intf_ops->onebox_free_pkt(nb_deploy, 0);
	
	return ONEBOX_STATUS_SUCCESS;
}


#ifdef USE_WORKQUEUES
/*
 * deferred_rx_packet_parser - Bottom half handler for interrupt
 *
 * @work - packet receive work
 *
 * Receives Data and Management frame from the interrupt handler.
 */
void deferred_rx_packet_parser(struct work_struct *work)
{
	uint8 s = ONEBOX_STATUS_SUCCESS;

	struct onebox_priv  *adapter   = NULL;
	netbuf_ctrl_block_t *netbuf_cb = NULL;

	adapter = container_of(work ,struct onebox_priv ,defer_work);
	
	while (adapter->os_intf_ops->onebox_netbuf_queue_len(&adapter->deferred_rx_queue)) {
		netbuf_cb = adapter->os_intf_ops->onebox_dequeue_pkt(&adapter->deferred_rx_queue);
		if (!netbuf_cb) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("HAL : Invalid netbuf_cb from deferred_rx_queue \n"));
			continue;
		}

		s = deploy_packet_to_assets(adapter, netbuf_cb);
		if (s) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("FAILED TO DEPLOY packet[%p]\n", netbuf_cb));
			return;
		}
	}
	return;
}
EXPORT_SYMBOL(deferred_rx_packet_parser);
#endif

#ifdef USE_TASKLETS
void deferred_rx_tasklet(unsigned long d)
{
	uint8 s = ONEBOX_STATUS_SUCCESS;

	PONEBOX_ADAPTER adapter = (PONEBOX_ADAPTER)d;
	netbuf_ctrl_block_t *netbuf_cb = NULL;

	while (adapter->os_intf_ops->onebox_netbuf_queue_len(&adapter->deferred_rx_queue)) {

		netbuf_cb = adapter->os_intf_ops->onebox_dequeue_pkt(&adapter->deferred_rx_queue);
		if (!netbuf_cb) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("HAL : Invalid netbuf_cb from deferred_rx_queue \n"));
			continue;
		}

		s = deploy_packet_to_assets(adapter, netbuf_cb);
		if (s) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("FAILED TO DEPLOY packet[%p]\n", netbuf_cb));
			return s;
		}
	}

	return;
}
EXPORT_SYMBOL(deferred_rx_tasklet);
#endif

/**
 * This function read frames from the SD card.
 *
 * @param  Pointer to driver adapter structure.  
 * @param  Pointer to received packet.  
 * @param  Pointer to length of the received packet.  
 * @return 0 if success else -1. 
 */
ONEBOX_STATUS onebox_read_pkt(PONEBOX_ADAPTER adapter)
{
	struct driver_assets *d_assets = adapter->d_assets;
	uint8  num_blks      = 0; 
	uint8  protocol	     = 0;
	uint8  reg_value     = 0;
	uint8  *rx_data      = NULL;
	uint32 rcv_pkt_len   = 0;
	uint8  unaggr_pkt = 0;


	ONEBOX_STATUS status = ONEBOX_STATUS_SUCCESS;
	netbuf_ctrl_block_t *netbuf_recv_pkt = NULL;

	FUNCTION_ENTRY(ONEBOX_ZONE_DATA_RCV);
	
	num_blks = ((adapter->interrupt_status & 1) | ((adapter->interrupt_status >> 4) << 1));
	//ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,	(TEXT("%s: Num of blocks =%d: \n"), __func__, num_blks));
	
	/* In case of wlan only mode avoiding the cmd52 read and getting the number of blocks info
 	 * from the host interrupt status register. In wlan only mode else case will be hit 
 	 */	 
	if(!num_blks)
	{
		/* Read length from SDIO_REG_ABORT_FEEDBACK register */
		status = adapter->osd_host_intf_ops->onebox_read_register(adapter,
				SDIO_RX_NUM_BLOCKS_REG,
				&reg_value);
		if (status != ONEBOX_STATUS_SUCCESS) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
					(TEXT("%s: Failed to read pkt length from the card: \n"),
					 __func__));
			if (adapter->host_intf_type == HOST_INTF_SDIO) {
#ifdef USE_SDIO_INTF
			onebox_sdio_abort_handler(adapter);
#endif
			}
			return status;
		}
		protocol = (reg_value >> 5);		/* 3 MSB indicate protocol type */
		num_blks = (reg_value & 0x1F);		/* 5 LSB indicate num of blocks */
	}
	else
	{
		protocol = WLAN_PKT;
		#ifdef SDIO_DEBUG	
		adapter->total_cmd52_skipped++;
		#endif
	}

//  check - 1
	if (adapter->stop_card_write == 2) {
		/* 
		 * acknowledging pkt_pending interrupt or masking it
		 * is required. This needs CMD53 write to TA processor
		 * registers. Since already CMD53 failed, it might not work.
		 *
		 * if stop_card_write == 2 is not due to CMD53 failure, then
		 * interrupt can be acknowledge.
		 * Lets try to acknowledge interrupt also.
		 *
		 * if acknowledge is unsuccessful, we need to try to deregister interrupt handler.
		 */

		status = adapter->osi_host_intf_ops->onebox_ack_interrupt(adapter, 
								(1 << SD_MSDU_PACKET_PENDING));
		if (status != ONEBOX_STATUS_SUCCESS) {
			status = adapter->osd_host_intf_ops->onebox_deregister_irq(adapter);
		}
		return ONEBOX_STATUS_FAILURE;
	}

	if (unlikely(!num_blks)) {
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
		             (TEXT("%s:Fail, Number of blocks to be read is 0 \n"),__func__));
		adapter->stop_card_write = 2;
		/* 
		 * acknowledging pkt_pending interrupt or masking it
		 * is needed.
		 */
		return panic_stop_all_activites(adapter, SD_MSDU_PACKET_PENDING);
	}

	if((d_assets->ta_aggr < 2) || ((protocol == BT_PKT) || (protocol == ZIGB_PKT))) {
		unaggr_pkt = 1;
	}
	rcv_pkt_len = num_blks * 256; 		//Each block size is 256 bytes 

	if(adapter->flashing_mode_on) {
		protocol = COEX_PKT;
	}
	if(unaggr_pkt) {

		if(protocol == ZIGB_PKT && num_blks > 1) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("*****ERROR: ZIGB RECVD MULTIPLE BLOCKS %d ******",num_blks));
		}

		netbuf_recv_pkt = adapter->os_intf_ops->onebox_alloc_skb(rcv_pkt_len);
		if (netbuf_recv_pkt == NULL) {	
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("HAL :  Unable to allocate NETBUFCB\n"));
			return -ENOMEM;
		} 

		netbuf_recv_pkt->len = rcv_pkt_len;
		netbuf_recv_pkt->rx_pkt_type = protocol;

		adapter->os_intf_ops->onebox_add_data_to_skb(netbuf_recv_pkt, rcv_pkt_len);
		
		rx_data = netbuf_recv_pkt->data;
	} else {
		rx_data = adapter->DataRcvPacket;
	}
					// read_packet_from_device
	status = adapter->osi_host_intf_ops->onebox_host_intf_read_pkt(adapter,
	                                                              rx_data,
																																rcv_pkt_len);
	if (d_assets->card_state != GS_CARD_ABOARD) {
		if (netbuf_recv_pkt)
			adapter->os_intf_ops->onebox_free_pkt(netbuf_recv_pkt, 0);
			return ONEBOX_STATUS_FAILURE;
	}
	if (status != ONEBOX_STATUS_SUCCESS) {
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
		             (TEXT("%s: Failed to read packet(complete pkt) from the card: \n"),
		             __func__));
		if (adapter->host_intf_type == HOST_INTF_SDIO) {
#ifdef USE_SDIO_INTF
			onebox_sdio_abort_handler(adapter);
#endif
		}
		return panic_stop_all_activites(adapter, SD_MSDU_PACKET_PENDING);
	}

	if(protocol == ZIGB_PKT){
#ifndef USE_INTCONTEXT
		adapter->os_intf_ops->onebox_netbuf_queue_tail(&adapter->deferred_rx_queue,
							       netbuf_recv_pkt->pkt_addr);
#ifdef USE_WORKQUEUES
		adapter->os_intf_ops->onebox_queue_work(adapter->int_work_queue, 
							&adapter->defer_work);
#endif
#ifdef USE_TASKLETS
		adapter->os_intf_ops->onebox_tasklet_sched(&adapter->int_bh_tasklet);
#endif
#else
		status = deploy_packet_to_assets(adapter, netbuf_recv_pkt);
		if (status) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("FAILED TO DEPLOY packet[%p]\n", netbuf_recv_pkt));
			return ONEBOX_STATUS_FAILURE;
		}
#endif
	}
	else if(unaggr_pkt) {
		/* Handling unaggregated packets, in BT/WLAN protocols */
		process_unaggregated_pkt(adapter, netbuf_recv_pkt, rcv_pkt_len);
	}
 	else {
		/* Handling aggregated packets, in BT/WLAN protocols */
		process_lmac_aggregated_pkt(adapter, rcv_pkt_len, protocol);
	}
	FUNCTION_EXIT(ONEBOX_ZONE_INFO);

	return ONEBOX_STATUS_SUCCESS;
}
