/**
 * @file onebox_wlan_ioctl.c
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
 * This file contians the code for handling ioctls.
 */

#include "wlan_common.h"
#include "onebox_wlan_core.h"

#define RSI_IAP_INIT 0
#define RSI_MFI_WRITE_CHALLENGE 1
#define RSI_MFI_READ_CERTIFICATE 2


ONEBOX_STATUS onebox_iap_config(WLAN_ADAPTER adapter, uint8 type, struct mfi_challenge *mfi)
{
	onebox_mac_frame_t *mgmt_frame;
	netbuf_ctrl_block_t *netbuf_cb = NULL;
	ONEBOX_STATUS status = ONEBOX_STATUS_SUCCESS;
	uint16 *frame_body;
	struct driver_assets *d_assets = adapter->d_assets;
	uint8 extnd_size = 0;
	uint16 buf_len = 0;

	if (type == RSI_IAP_INIT || type == RSI_MFI_READ_CERTIFICATE) {
		netbuf_cb = adapter->os_intf_ops->onebox_alloc_skb(FRAME_DESC_SZ);
		buf_len = FRAME_DESC_SZ;
	} else if (type == RSI_MFI_WRITE_CHALLENGE) {
		netbuf_cb = adapter->os_intf_ops->onebox_alloc_skb(FRAME_DESC_SZ + sizeof(struct mfi_challenge));
		buf_len = FRAME_DESC_SZ + sizeof(struct mfi_challenge);
 	} else {
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("%s: Invalid type\n"), __func__));
		return ONEBOX_STATUS_FAILURE;
	}
		
	if (netbuf_cb == NULL) { 
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("%s: Unable to allocate skb\n"), __func__));
		return ONEBOX_STATUS_FAILURE;
	}
   
	adapter->os_intf_ops->onebox_add_data_to_skb(netbuf_cb, buf_len);
	extnd_size = ((unsigned long )netbuf_cb->data & 0x3f);  /* To make it 64bit aligned buffer, to avoid mmc timeout interrupt */
	adapter->os_intf_ops->onebox_change_hdr_size(netbuf_cb, extnd_size);
	adapter->os_intf_ops->onebox_memset(netbuf_cb->data, 0, (FRAME_DESC_SZ + extnd_size));

	mgmt_frame = (onebox_mac_frame_t *)&netbuf_cb->data[0];
	adapter->os_intf_ops->onebox_memset(mgmt_frame, 0, buf_len);

	mgmt_frame->desc_word[0] = ONEBOX_CPU_TO_LE16(((buf_len - FRAME_DESC_SZ) + extnd_size) 
	                                              | (COEX_TX_Q << 12));
	netbuf_cb->tx_pkt_type = COEX_Q;
	mgmt_frame->desc_word[1] = ONEBOX_CPU_TO_LE16(IAP_CONFIG);
	//mgmt_frame->desc_word[2] = ONEBOX_CPU_TO_LE16(extnd_size);
	mgmt_frame->desc_word[3] = ONEBOX_CPU_TO_LE16(type);

	if (type == RSI_MFI_WRITE_CHALLENGE) {
		if (mfi == NULL) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("%s: MFI challenge is NULL!!!\n"), __func__));
			kfree_skb(netbuf_cb->pkt_addr);
			return ONEBOX_STATUS_FAILURE;
		}
		frame_body = (uint16 *)&netbuf_cb->data[16];
		adapter->os_intf_ops->onebox_memcpy(frame_body, mfi->challenge_data, sizeof(struct mfi_challenge));
		adapter->core_ops->onebox_dump(ONEBOX_ZONE_ERROR,	(uint8 *)netbuf_cb->data, netbuf_cb->len) ;
	}

	adapter->core_ops->onebox_dump(ONEBOX_ZONE_DEBUG,
					(uint8 *)mgmt_frame,
					 buf_len) ;
	status = adapter->onebox_send_pkt_to_coex(d_assets, netbuf_cb, COEX_Q);
	return status;
}

/**
 *  This function will return index of a given ioctl command.
 *  And the output parameter private indicates whether the given
 *  ioctl is a standard ioctl command or a private ioctl.
 *   0: Standard
 *   1: Private ioctl
 *  -1: Illiegal ioctl
 *
 * @param  value of the ioctl command, input to this function
 * @param  Indicates whether the ioctl is private or standart, output pointer
 * @return returns index of the ioctl
 */

static int get_ioctl_index(int cmd, int *private)
{
	int index = 0;
	*private = 0;

	if ( (cmd >= SIOCIWFIRSTPRIV) && (cmd <= SIOCIWLASTPRIV)) 
	{
		/* Private IOCTL */
		index = cmd - SIOCIWFIRSTPRIV;
		*private = 1;
	} 
	else if ((cmd >= 0x8B00) && (cmd <= 0x8B2D)) 
	{
		/* Standard IOCTL */
		index = cmd - 0x8B00;
		*private = 0;
	} 
	else 
	{
		*private = -1;
	}
	return index;
}

/**
 * This function handles the ioctl for deleting a VAP.
 * @param  Pointer to the ieee80211com structure
 * @param  Pointer to the ifreq structure
 * @param  Pointer to the netdevice structure
 * @return Success or failure  
 */
int
ieee80211_ioctl_delete_vap(struct ieee80211com *ic, struct ifreq *ifr, struct net_device *mdev)
{
	struct ieee80211vap *vap = NULL;
	struct ieee80211_clone_params cp;
	char name[IFNAMSIZ];
	uint8_t wait_for_lock = 0;
	uint8_t vap_del_flag = 0;
	//WLAN_ADAPTER w_adapter = (WLAN_ADAPTER)netdev_priv(mdev);

	if (!capable(CAP_NET_ADMIN))
	{
		return -EPERM;
	}

	if (copy_from_user(&cp, ifr->ifr_data, sizeof(cp)))
	{
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Copy from user failed\n")));
		return -EFAULT;
	}

	strncpy(name, cp.icp_parent, sizeof(name));

	TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
	{
		ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("User given vap name =%s and list of vap names=%s\n"), name, vap->iv_ifp->name));
		if (!strcmp(vap->iv_ifp->name, name)) 
		{
			//w_adapter->net80211_ops->onebox_ifdetach(ic);
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Deleting vap\n")));
			ic->ic_vap_delete(vap, wait_for_lock);
			vap_del_flag =1;
			break;
		}
	}
	if(vap_del_flag)
	{
			return 0;
	}
	else
	{
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Invalid VAP name given for deletion\n")));
		return -1;
	}
}

/**
 * This function creates a virtual ap.This is public as it must be
 * implemented outside our control (e.g. in the driver).
 * @param  Pointer to the ieee80211com structure
 * @param  Pointer to the ifreq structure
 * @param  Pointer to the netdevice structure
 * @return Success or failure  
 */
int ieee80211_ioctl_create_vap(struct ieee80211com *ic, struct ifreq *ifr,
                                struct net_device *mdev)
{
	struct ieee80211_clone_params cp;
	struct ieee80211vap *vap;
	char name[IFNAMSIZ];
	WLAN_ADAPTER w_adapter = (WLAN_ADAPTER)netdev_priv(mdev);
	//uint8 vap_id;

	if (!capable(CAP_NET_ADMIN))
	{
		return -EPERM;
	}
	if (copy_from_user(&cp, ifr->ifr_data, sizeof(cp)))
	{
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Copy from user failed\n")));
		return -EFAULT;
	}

	strncpy(name, cp.icp_parent, sizeof(name));
	if((cp.icp_opmode == 1) && (w_adapter->beacon_recv_disable == 1))
	{
		/* If Vap is created in station mode and if beacon_receive is disabled, we need to enable it. */
		w_adapter->devdep_ops->conf_beacon_recv(w_adapter, 0);
	}
#if 0
	vap_id = w_adapter->os_intf_ops->onebox_extract_vap_id(name);
	TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next)
	{
		if(vap && (vap->hal_priv_vap->vap_id == vap_id)) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Virtual Interface with similar name is already created\n")));
			return -EFAULT;
		}
	}
#endif
	ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("Name for vap creation is: %s rtnl %d\n"), name, rtnl_is_locked()));
	/*  */ //Check 3,6,7 param whether it should be 0 or not
	vap = w_adapter->core_ops->onebox_create_vap(ic, name, 0, cp.icp_opmode, cp.icp_flags, NULL, w_adapter->mac_addr);
	if (vap == NULL)
	{
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("VAP = NULL\n")));
		return -EIO;
	}

	/*
 	 * If AP is the first vap and the user has not set a reg domain,
 	 * setting the reg domain to default FCC / United States.
 	 */
#ifdef ONEBOX_CONFIG_CFG80211
	TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) {
		if (vap->iv_opmode == IEEE80211_M_HOSTAP && 
		    ic->ic_regdomain.country == CTRY_DEFAULT) {
			ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("Setting regdom to US\n")));
			ic->ic_regdomain.country = CTRY_UNITED_STATES;
			ic->ic_regdomain.location = ' ';
			ic->ic_regdomain.isocc[0] = 'U';
			ic->ic_regdomain.isocc[1] = 'S';
			ic->ic_regdomain.pad[0] = 0;			
			w_adapter->net80211_ops->onebox_media_init(ic);
		}
		break;
	}
#endif

	/* return final device name */
	strncpy(ifr->ifr_name, vap->iv_ifp->name, IFNAMSIZ);
	return 0;
}

bool check_valid_bgchannel(uint16 *data_ptr, uint8_t supported_band)
{
	uint8_t ii, jj;
	uint8_t num_chan = *((uint8 *)(data_ptr) + 6) ;
	uint16_t chan_5g[] = {36, 40, 44, 48, 149, 153, 157, 161, 165};
	uint16_t chan_check[num_chan];

	memcpy(chan_check, (uint16 *)(data_ptr + 6), 2*num_chan);

	if (!supported_band) {
		for (ii = 0; ii < num_chan; ii++) {
			for (jj = 0; jj < num_chan; jj++) {
				if (chan_check[ii] == chan_5g[jj]) {
					ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("ERROR: Trying to program 5GHz channel on a card supporting only 2.4GHz\n")));
					return false;
				}
			}
		}
	}

	return true;
}

static void send_sleep_req_in_per_mode(WLAN_ADAPTER w_adapter, uint8 *data)
{
	onebox_mac_frame_t *mgmt_frame;
	ONEBOX_STATUS status = 0;
	uint8  pkt_buffer[MAX_MGMT_PKT_SIZE];
	struct pwr_save_params ps_params_ioctl;//Parameters to store IOCTL parameters from USER
	struct driver_assets *d_assets = w_adapter->d_assets;
	uint8 request =1;

	mgmt_frame = (onebox_mac_frame_t *)pkt_buffer;
	memcpy(&ps_params_ioctl, data, sizeof(struct pwr_save_params));

	w_adapter->os_intf_ops->onebox_memset(mgmt_frame, 0, MAX_MGMT_PKT_SIZE);
	mgmt_frame->desc_word[0] = ONEBOX_CPU_TO_LE16(sizeof(mgmt_frame->u.ps_req_params) | (ONEBOX_WIFI_MGMT_Q << 12));
	mgmt_frame->desc_word[1] = ONEBOX_CPU_TO_LE16(WAKEUP_SLEEP_REQUEST);

	mgmt_frame->u.ps_req_params.ps_req.sleep_type = ps_params_ioctl.sleep_type; //LP OR ULP
	mgmt_frame->u.ps_req_params.listen_interval = ps_params_ioctl.listen_interval;
	mgmt_frame->u.ps_req_params.ps_req.sleep_duration = ps_params_ioctl.deep_sleep_wakeup_period;
	mgmt_frame->u.ps_req_params.ps_req.ps_en = ps_params_ioctl.ps_en;
	mgmt_frame->u.ps_req_params.ps_req.connected_sleep = DEEP_SLEEP;

	if(!ps_params_ioctl.ps_en) {
		mgmt_frame->desc_word[0] |= 1 << 15; //IMMEDIATE WAKE UP
	}

	ONEBOX_DEBUG(ONEBOX_ZONE_PWR_SAVE, (" <==== Sending Power save request =====> In %s Line %d  \n", __func__, __LINE__));
	status = w_adapter->devdep_ops->onebox_send_internal_mgmt_frame(w_adapter,
			(uint16 *)mgmt_frame,
			FRAME_DESC_SZ + sizeof(mgmt_frame->u.ps_req_params));
	if (d_assets->host_intf_type == HOST_INTF_SDIO) {
		msleep (2);
		ONEBOX_DEBUG (ONEBOX_ZONE_ERROR,
				(TEXT ("Writing disable to wakeup register\n")));
		status =
			d_assets->onebox_common_write_register (d_assets, 0, SDIO_WAKEUP_REG,
					&request);
	}
	return ;
}

ONEBOX_STATUS send_wowlan_params(struct ieee80211vap *vap, WLAN_ADAPTER w_adapter)
{
	onebox_mac_frame_t *mgmt_frame;
	ONEBOX_STATUS status = 0;
	uint8  pkt_buffer[MAX_MGMT_PKT_SIZE];

	mgmt_frame = (onebox_mac_frame_t *)pkt_buffer;
	w_adapter->os_intf_ops->onebox_memset(mgmt_frame, 0, MAX_MGMT_PKT_SIZE);
	mgmt_frame->desc_word[0] = ONEBOX_CPU_TO_LE16(sizeof(mgmt_frame->u.wowlan_params) | (ONEBOX_WIFI_MGMT_Q << 12));
	mgmt_frame->desc_word[1] = ONEBOX_CPU_TO_LE16(WOWLAN_CONFIG_PARAMS);
	memcpy(&mgmt_frame->u.wowlan_params, &vap->hal_priv_vap->wowlan_params, sizeof(struct wowlan_config));

	ONEBOX_DEBUG(ONEBOX_ZONE_PWR_SAVE, (" <==== Sending WOWLAN PARAMS =====> In %s Line %d  \n", __func__, __LINE__));
	status = w_adapter->devdep_ops->onebox_send_internal_mgmt_frame(w_adapter,
			(uint16 *)mgmt_frame,
			FRAME_DESC_SZ + sizeof(mgmt_frame->u.wowlan_params));
	return status;

}

static ONEBOX_STATUS wlan_deregister_fw(WLAN_ADAPTER w_adapter)
{
	onebox_mac_frame_t *mgmt_frame;
	netbuf_ctrl_block_t *netbuf_cb = NULL;
	ONEBOX_STATUS status = ONEBOX_STATUS_SUCCESS;
	struct driver_assets *d_assets = w_adapter->d_assets;

	ONEBOX_DEBUG(ONEBOX_ZONE_INFO,
			(TEXT("===> Deregister WLAN FW <===\n")));

	netbuf_cb = w_adapter->os_intf_ops->onebox_alloc_skb(FRAME_DESC_SZ);
	if(netbuf_cb == NULL)
	{	
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("%s: Unable to allocate skb\n"), __func__));
		status = ONEBOX_STATUS_FAILURE;
		return status;
	}
	w_adapter->os_intf_ops->onebox_add_data_to_skb(netbuf_cb, FRAME_DESC_SZ);
    /* Aligning the netbuf->data pointer to avoid issues on embedded platforms */

	mgmt_frame = (onebox_mac_frame_t *)netbuf_cb->data;

	w_adapter->os_intf_ops->onebox_memset(mgmt_frame, 0, FRAME_DESC_SZ);

	/* FrameType*/
	mgmt_frame->desc_word[1] = ONEBOX_CPU_TO_LE16(WLAN_DE_REGISTER);
#define IMMEDIATE_WAKEUP 1
	mgmt_frame->desc_word[0] = ((ONEBOX_WIFI_MGMT_Q << 12)| (IMMEDIATE_WAKEUP << 15));
	netbuf_cb->tx_pkt_type = WLAN_TX_M_Q;

	ONEBOX_DEBUG(ONEBOX_ZONE_DEBUG, (TEXT("<==== DEREGISTER FRAME ====>\n")));
	w_adapter->core_ops->onebox_dump(ONEBOX_ZONE_DEBUG, (PUCHAR)mgmt_frame, FRAME_DESC_SZ);
	status = w_adapter->onebox_send_pkt_to_coex(w_adapter->d_assets, netbuf_cb, WLAN_Q);
	if (status != ONEBOX_STATUS_SUCCESS) 
	{ 
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,(TEXT 
			     ("%s: %d Failed To Write The Packet\n"),__func__, __LINE__));
	}

	if(d_assets->host_intf_type == HOST_INTF_SDIO)
		w_adapter->os_intf_ops->onebox_free_pkt(netbuf_cb, 0);
	return status;
}


/**
 *  Calls the corresponding (Private) IOCTL functions
 *
 * @param  pointer to the net_device
 * @param  pointer to the ifreq
 * @param  value of the ioctl command, input to this function
 * @return returns 0 on success otherwise returns the corresponding 
 * error code for failure
 */
int onebox_ioctl(struct net_device *dev,struct ifreq *ifr, int cmd)
{
	WLAN_ADAPTER w_adapter = (WLAN_ADAPTER)netdev_priv(dev);
	struct ieee80211com *ic = &w_adapter->vap_com;
	struct iwreq *wrq = (struct iwreq *)ifr;
	int index, priv, ret_val=0;
	struct ieee80211_node *ni = NULL;
	struct ieee80211vap *vap = NULL;
	uint8_t macaddr[IEEE80211_ADDR_LEN] = {0};
	unsigned int value = 0;
	unsigned int channel = 1;
	unsigned int set_band = BAND_2_4GHZ;
	unsigned int status;
	onebox_mac_frame_t *mgmt_frame;
	struct test_mode test;
	struct get_info *getinfo;
  int found = 0;
  int8_t *ant_gain;
  struct wowlan_config *wowlan_params = NULL;
	struct driver_assets *d_assets = w_adapter->d_assets;
	uint32_t enable = 0;
	uint32_t protocol_status = 0;
	driver_params_t driver_params;
	unsigned short country_code;
	uint16_t rx_filter_word;

	w_adapter->os_intf_ops->onebox_memset(&driver_params, 0, sizeof(driver_params_t));
	ONEBOX_DEBUG(ONEBOX_ZONE_INFO, ("In onebox_ioctl function\n"));
	/* Check device is present or not */
	if (!netif_device_present(dev))
	{
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Device not present\n")));
		return -ENODEV;
	}

	/* Get the IOCTL index */
	index = get_ioctl_index(cmd, &priv);

	/*vap creation command*/
	switch(cmd)
	{
		case RSI_WATCH_IOCTL:
		{
			wrq->u.data.length = 4;
			if (w_adapter->buffer_full)
			{
				w_adapter->watch_bufferfull_count++;

				if (w_adapter->watch_bufferfull_count > 10) /*  : not 10, should dependent on time */
				{
					/* Incase of continous buffer full, give the last beacon counter */
					ret_val = copy_to_user(wrq->u.data.pointer, &w_adapter->total_beacon_count, 4);
					return ret_val;
				}
			}

			ret_val = copy_to_user(wrq->u.data.pointer, &w_adapter->total_beacon_count, 4);
			return ret_val;
		} 
		break;
		case ONEBOX_VAP_CREATE:
		{
			if ((w_adapter->Driver_Mode != WIFI_MODE_ON) && (w_adapter->Driver_Mode != SNIFFER_MODE))
			{
				ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Driver Mode is not in WIFI_MODE vap creation is not Allowed\n")));	
				return ONEBOX_STATUS_FAILURE;
			}
			ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (" VAP Creation \n"));
			ret_val = ieee80211_ioctl_create_vap(ic, ifr, dev);

			if(ret_val == 0)
			{
				ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("Created VAP with dev name:%s\n"),ifr->ifr_name));
			}
			return ret_val;
		}
		break;
		case ONEBOX_VAP_DELETE:
		{
			ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (" VAP delete \n"));
			ret_val = ieee80211_ioctl_delete_vap(ic, ifr, dev);
			if(ret_val == 0)
			{
				ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("Deleted VAP with dev name:%s\n"),ifr->ifr_name));
			}
			return ret_val;
		}
#define IS_RUNNING(ifp) \
((ifp->if_flags & IFF_UP) && (ifp->if_drv_flags & IFF_DRV_RUNNING))
		case SIOCSIFFLAGS:
		{
			ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("In SIOCSIFFLAGS case dev->flags =%x\n"), dev->if_flags));
			/* Not doing anything here */
			if (IS_RUNNING(dev)) 
			{
				/* Nothing to be done here */
			} 
			else if (dev->if_flags & IFF_UP) 
			{
				dev->if_drv_flags |= IFF_DRV_RUNNING;
				ieee80211_start_all(ic);
			} 
			else 
			{
				dev->if_drv_flags &= ~IFF_DRV_RUNNING;
			} 
			return ret_val;
		}
		break;
		case SIOCGPROTOCOL:
		{
			enable = (uint32_t)wrq->u.data.flags ;
			protocol_status = wrq->u.data.length;

			if (enable) {
				if (protocol_status & WLAN_PROTOCOL) {
					if(!(d_assets->protocol_enabled & WLAN_PROTOCOL) && 
							(d_assets->techs[WLAN_ID].fw_state == FW_ACTIVE)) {
						w_adapter->fsm_state = FSM_LOAD_BOOTUP_PARAMS ;
						d_assets->protocol_enabled |= WLAN_PROTOCOL;
						if (onebox_load_bootup_params(w_adapter) != ONEBOX_STATUS_SUCCESS) {
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,(TEXT ("%s: Failed to load bootup parameters\n"), 
										__func__));
							return -1;
						}

						ONEBOX_DEBUG(ONEBOX_ZONE_FSM,
								(TEXT("%s: BOOTUP Parameters loaded successfully\n"),__func__));
					} else {
						ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
								(TEXT("%s: %d Error: WLAN firmware is not active\n"),__func__, __LINE__));
						return -1;
					}
				}

#ifdef BT_ENABLE
				if (protocol_status & BT_PROTOCOL) { 
					if(!(d_assets->protocol_enabled & BT_PROTOCOL) &&
							(d_assets->techs[BT_ID].fw_state == FW_ACTIVE) && 
							(d_assets->techs[BT_ID].drv_state != MODULE_ACTIVE)
					  ) {
						d_assets->techs[BT_ID].inaugurate(d_assets);
						d_assets->protocol_enabled |= BT_PROTOCOL;
					} else {
						ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
								(TEXT("%s: %d Error: BT firmware is not active\n"),__func__, __LINE__));
						return -1;
					}
				}
#endif
#ifdef ZIGB_ENABLE
				if (protocol_status & ZIGB_PROTOCOL) { 
					if (!(d_assets->protocol_enabled & ZIGB_PROTOCOL) &&
							(d_assets->techs[ZB_ID].fw_state == FW_ACTIVE) && 
							(d_assets->techs[ZB_ID].drv_state != MODULE_ACTIVE)) {
						d_assets->techs[ZB_ID].inaugurate(d_assets);
						d_assets->protocol_enabled |= ZIGB_PROTOCOL;
					} else {
						ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
								(TEXT("%s: %d Error: ZIGB firmware is not active\n"),__func__, __LINE__));
						return -1;
					}
				}
#endif
			} else { /*Disable the required protocol */

				if ((protocol_status & WLAN_PROTOCOL) &&
						(d_assets->protocol_enabled & WLAN_PROTOCOL)) {
					TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) {
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("Deleting vap\n")));
						ic->ic_vap_delete(vap, 0);
					}
					core_net80211_detach(w_adapter);
					d_assets->techs[WLAN_ID].tx_intention = 1;
					d_assets->update_tx_status(d_assets, WLAN_ID);

					if(!d_assets->techs[WLAN_ID].tx_access) {
						ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Waiting for tx_acces from common hal cmntx %d\n"), d_assets->common_hal_tx_access));
						d_assets->techs[WLAN_ID].deregister_flags = 1;
						ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Waiting event %s Line %d\n"), __func__, __LINE__));
						if (wait_event_timeout((d_assets->techs[WLAN_ID].deregister_event), (d_assets->techs[WLAN_ID].deregister_flags == 0), msecs_to_jiffies(6000) )) {
							if(!d_assets->techs[WLAN_ID].tx_access) {
								ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("In %s Line %d unable to get access \n"), __func__, __LINE__));
								return -1;
							}
						} else {
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("ERR: In %s Line %d Initialization of WLAN Failed as Wlan TX access is not granted from Common Hal \n"), __func__, __LINE__));
							return -1;
						}
					}
					wlan_deregister_fw(w_adapter);
					d_assets->techs[WLAN_ID].tx_intention = 0;
					d_assets->techs[WLAN_ID].tx_access = 0;
					d_assets->update_tx_status(d_assets, WLAN_ID);
					d_assets->techs[WLAN_ID].fw_state = FW_INACTIVE;
					d_assets->protocol_enabled &= ~WLAN_PROTOCOL;
				}

#ifdef BT_ENABLE
				if ((protocol_status & BT_PROTOCOL) &&
						(d_assets->protocol_enabled & BT_PROTOCOL) &&
						(d_assets->techs[BT_ID].drv_state == MODULE_ACTIVE)) {
					d_assets->techs[BT_ID].disconnect(d_assets);
					d_assets->protocol_enabled &= ~BT_PROTOCOL;
				}
#endif
#ifdef ZIGB_ENABLE
				if ((protocol_status & ZIGB_PROTOCOL) &&
						(d_assets->protocol_enabled & ZIGB_PROTOCOL) &&
						(d_assets->techs[ZB_ID].drv_state == MODULE_ACTIVE)) {
					d_assets->techs[ZB_ID].disconnect(d_assets);
					d_assets->protocol_enabled &= ~ZIGB_PROTOCOL;
				}
#endif

			}
		}
		break;
		case ONEBOX_HOST_IOCTL:
		{
			if(w_adapter->Driver_Mode == WIFI_MODE_ON)
			{
				value = wrq->u.data.length;
				switch((unsigned char)wrq->u.data.flags)
				{
					case PER_RECEIVE_STOP:
						w_adapter->recv_stop = 1;
						w_adapter->rx_running = 0;
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO,("PER_RECEIVE_STOP\n"));
					case PER_RECEIVE:
						if (!w_adapter->rx_running)
						{
							if(!(w_adapter->core_ops->onebox_stats_frame(w_adapter)))
							{
								w_adapter->rx_running = 1;
								if (w_adapter->recv_stop)
								{
									w_adapter->recv_stop = 0;
									w_adapter->rx_running = 0;
									return ONEBOX_STATUS_SUCCESS;
								}  
								status = w_adapter->os_intf_ops->onebox_wait_event(&(w_adapter->stats_event), EVENT_WAIT_FOREVER);
								if (status < 0) {
										ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s line no %d : wait event failed %d\n", __func__, __LINE__, status));
										return status;
								}
								w_adapter->os_intf_ops->onebox_reset_event(&(w_adapter->stats_event));
								ret_val = copy_to_user(wrq->u.data.pointer, &w_adapter->sta_info, sizeof(per_stats));
								return ret_val;
							}
						}
						else
						{
							status = w_adapter->os_intf_ops->onebox_wait_event(&(w_adapter->stats_event), EVENT_WAIT_FOREVER);
							if (status < 0) {
									ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s line no %d : wait event failed %d\n", __func__, __LINE__, status));
									return status;
							}
							w_adapter->os_intf_ops->onebox_reset_event(&(w_adapter->stats_event));
							ret_val = copy_to_user(wrq->u.data.pointer, &w_adapter->sta_info, sizeof(per_stats));
							return ret_val;
						}
						break;
					case SET_BEACON_INVL:
						if (IEEE80211_BINTVAL_MIN_AP <= value &&
								value <= IEEE80211_BINTVAL_MAX) 
						{
							ic->ic_bintval = ((value + 3) & ~(0x3));
						} 
						else
						{
							ret_val = EINVAL;
						}
						w_adapter->beacon_interval = ONEBOX_CPU_TO_LE16(((value + 3) & ~(0x3)));
						break;
					case SET_ENDPOINT:
						value = ((unsigned short)wrq->u.data.flags >> 8); //endpoint value 
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("ENDPOINT type is : %d \n"),value));
						if (!w_adapter->band_supported) {
							if (value == 2 || value == 3) {
								ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("ERROR: 5GHz endpoint not supported\n")));
								return -EINVAL;
							}
						}
						w_adapter->endpoint = value;
						w_adapter->devdep_ops->onebox_program_bb_rf(w_adapter);
						break;
					case ANT_SEL:
						if (w_adapter->antenna_diversity) {
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, 
								     (TEXT("ERROR: Trying to select\
									   antenna when antenna diversity\
									   feature is enabled\n")));
							return -EINVAL;
						}
						value = ((unsigned short)wrq->u.data.flags >> 8); //endpoint value 
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("ANT_SEL value is : %d \n"),value));
						w_adapter->devdep_ops->onebox_program_ant_sel(w_adapter, value);
						break;
					case SET_BGSCAN_PARAMS:
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("<<< BGSCAN >>>\n")));
//						onebox_send_bgscan_params(w_adapter, wrq->u.data.pointer , 0);
						TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
						{ 
							ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT(" mac addr=%02x:%02x:%02x:%02x:%02x:%02x\n"), macaddr[0], 
										macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]));
							if(vap->iv_opmode == IEEE80211_M_STA)
							{
								if (!check_valid_bgchannel(wrq->u.data.pointer, w_adapter->band_supported)) {
									ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Invalid channel in bg param set; 5GHz not supported by card\n")));
									return -EINVAL;
								}

								memset(&vap->hal_priv_vap->bgscan_params_ioctl, 0, sizeof(mgmt_frame->u.bgscan_params));
								memcpy(&vap->hal_priv_vap->bgscan_params_ioctl, wrq->u.data.pointer, sizeof(mgmt_frame->u.bgscan_params));
								vap->hal_priv_vap->bgscan_params_ioctl.bg_ioctl = 1;
								if(vap->iv_state == IEEE80211_S_RUN && (!w_adapter->sta_mode.delay_sta_support_decision_flag))
								{
									ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("Bgscan: In %s and %d \n"), __func__, __LINE__));
									if(onebox_send_bgscan_params(vap, wrq->u.data.pointer , 0))
                  {
									ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Bgscan: In %s and %d \n"), __func__, __LINE__));
                    return ONEBOX_STATUS_FAILURE;
                  }
								}
								else
								{
									ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Bgscan: In %s and %d \n"), __func__, __LINE__));
									return 0;
								}
								ni = w_adapter->net80211_ops->onebox_find_node(&vap->iv_ic->ic_sta, vap->iv_myaddr);
								if (ni == NULL)
								{    
									ONEBOX_DEBUG(ONEBOX_ZONE_OID, (TEXT("Ni is null vap node not found\n")));
									return ENOENT;
								}

								send_bgscan_probe_req(w_adapter, ni, 0);
								return 0;
							}
						}
						ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Issue IOCTL after vap creation in %s Line %d\n"), __func__, __LINE__));
						return -EINVAL;
						break;
					case DO_BGSCAN:
							ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("<<< DO BGSCAN IOCTL Called >>>\n")));
							TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
							{ 
								if(vap && (vap->iv_opmode == IEEE80211_M_STA))
								{
									ni = w_adapter->net80211_ops->onebox_find_node(&vap->iv_ic->ic_sta, vap->iv_myaddr);
									if (ni == NULL)
									{    
										ONEBOX_DEBUG(ONEBOX_ZONE_OID, (TEXT("Ni is null vap node not found\n")));
										return ENOENT;
									}
									if(vap && (vap->hal_priv_vap->bgscan_params_ioctl.bg_ioctl))
									{
										memcpy(&vap->hal_priv_vap->bgscan_params_ioctl.bg_cmd_flags, wrq->u.data.pointer, sizeof(uint16_t));
										if((vap->iv_state == IEEE80211_S_RUN))
										{
											send_bgscan_probe_req(w_adapter, ni, vap->hal_priv_vap->bgscan_params_ioctl.bg_cmd_flags);
										}
										return 0;
									}
									else
									{
										ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Issue this IOCTL only after issuing bgscan_params ioctl\n")));
										return -EINVAL;
									}
								}
							}
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Issue IOCTL after vap creation in %s Line %d\n"), __func__, __LINE__));
							return -EINVAL;

						break;
					case BGSCAN_SSID:
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("<<< BGSCAN SSID >>>\n")));
						TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
						{ 
							ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT(" mac addr=%02x:%02x:%02x:%02x:%02x:%02x\n"), macaddr[0], 
							macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]));
							if(vap->iv_opmode == IEEE80211_M_STA)
							{
								memcpy(&vap->hal_priv_vap->bg_ssid,  wrq->u.data.pointer, sizeof(vap->hal_priv_vap->bg_ssid));
								ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("SSID len is %d ssid is %s\n"), vap->hal_priv_vap->bg_ssid.ssid_len, vap->hal_priv_vap->bg_ssid.ssid));
							}
						}

						break;
#ifdef PWR_SAVE_SUPPORT
						case PS_REQUEST:
						{
							ONEBOX_DEBUG(ONEBOX_ZONE_PWR_SAVE, (TEXT("Name for vap creation is rtnl %d\n"),  rtnl_is_locked()));
							ONEBOX_DEBUG(ONEBOX_ZONE_PWR_SAVE, (TEXT("In %s Line %d issued PS_REQ ioctl \n"),__func__,__LINE__));
							TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
							{ 
								if(vap->iv_opmode == IEEE80211_M_STA){
									break;
								}
							}
							if(vap)
							{
								memcpy(&vap->hal_priv_vap->ps_params_ioctl, wrq->u.data.pointer, sizeof(vap->hal_priv_vap->ps_params));
								ONEBOX_DEBUG(ONEBOX_ZONE_PWR_SAVE, (TEXT("monitor interval %d\n"), vap->hal_priv_vap->ps_params_ioctl.monitor_interval));
								driver_ps.update_ta = 1;
								if((!vap->hal_priv_vap->ps_params_ioctl.ps_en))
								{
									d_assets->ps_en_ioctl = PS_DISABLE;
								}
								else
								{
									d_assets->ps_en_ioctl = PS_ENABLE;
								}
								update_pwr_save_status(vap, PS_ENABLE, IOCTL_PATH);
							}
							else
							{
													ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("ERROR: Give IOCTL after station vap Creation\n")));
													return -EINVAL;
							}
							break;
						}
						case UAPSD_REQ:
						{
							TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
							{ 
								if(vap->iv_opmode == IEEE80211_M_STA){
									break;
								}
							}
							if(vap && (vap->iv_state == IEEE80211_S_RUN))
							{
								memcpy(&vap->hal_priv_vap->uapsd_params_ioctl, wrq->u.data.pointer, sizeof(vap->hal_priv_vap->uapsd_params_ioctl));
							}
							else if(vap)
							{
								memcpy(&vap->hal_priv_vap->uapsd_params_ioctl, wrq->u.data.pointer, sizeof(vap->hal_priv_vap->uapsd_params_ioctl));
								ONEBOX_DEBUG(ONEBOX_ZONE_PWR_SAVE, (TEXT("acs %02x wakeup %02x \n"), vap->hal_priv_vap->uapsd_params_ioctl.uapsd_acs, vap->hal_priv_vap->uapsd_params_ioctl.uapsd_wakeup_period));
								memcpy(&vap->hal_priv_vap->uapsd_params_updated, &vap->hal_priv_vap->uapsd_params_ioctl, sizeof(vap->hal_priv_vap->uapsd_params_ioctl));
							}
							else
							{
								ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Give IOCTL after vap Creation\n")));
								return -EINVAL;
							}
							break;
						}
#endif
						case RESET_ADAPTER:
						{
							if (w_adapter->sc_nvaps > 1) 
								return -1;
							TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
							{ 
								if(vap->iv_opmode == IEEE80211_M_STA){
									break;
								}
							}
							if(vap)
							{
#if 0
								ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Copying ioctl values to updated \n")));
								if((&vap->hal_priv_vap->uapsd_params_ioctl) && (&vap->hal_priv_vap->uapsd_params_updated))
								{
									memcpy(&vap->hal_priv_vap->uapsd_params_updated, &vap->hal_priv_vap->uapsd_params_ioctl, sizeof(vap->hal_priv_vap->uapsd_params_ioctl));
								}
#endif
								ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Resetting the Adapter settings \n")));
								ni = vap->iv_bss; 
								if(ni && (vap->iv_state == IEEE80211_S_RUN))
								{
									ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Issuing sta leave cmd\n")));
									w_adapter->net80211_ops->onebox_ieee80211_sta_leave(ni);
								}
							}
							break;
						}
						case RX_FILTER:
						{
							memcpy(&w_adapter->rx_filter_word,wrq->u.data.pointer,sizeof(w_adapter->rx_filter_word));
							rx_filter_word = w_adapter->rx_filter_word;
							
									TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
									{
										if ( vap->iv_opmode == IEEE80211_M_STA ) {
											if ((vap->iv_state == IEEE80211_S_RUN) && (vap->iv_flags_ext & IEEE80211_FEXT_SWBMISS) ) {	
												rx_filter_word &= ~DISALLOW_BEACONS;
											}
										}
									}
									ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("Setting RX_FILTER %04x\n"), w_adapter->rx_filter_word));
									status = onebox_send_rx_filter_frame(w_adapter, rx_filter_word);
									if(status < 0)
									{
													ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Sending of RX filter frame failed\n")));
									}
									break;
						}
						case RF_PWR_MODE:
						{
							memcpy(&w_adapter->rf_pwr_mode, wrq->u.data.pointer, sizeof(w_adapter->rf_pwr_mode));	
							ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("Setting RF PWR MODE %04x\n"), w_adapter->rf_pwr_mode));
							ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("Setting RF PWR MODE %d\n"), w_adapter->rf_pwr_mode));
							break;
						}
						case RESET_PER_Q_STATS:
						{
							int q_num;
							ONEBOX_DEBUG(ONEBOX_ZONE_INFO,("Resetting WMM stats\n"));
							if (copy_from_user(&q_num, ifr->ifr_data, sizeof(q_num)))
							{
								ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Copy from user failed\n")));
								return -EFAULT;
							}
							if(q_num < MAX_HW_QUEUES) {
								w_adapter->total_tx_data_dropped[q_num] = 0;
								w_adapter->total_tx_data_sent[q_num] = 0;
							}else if(q_num == 15) {
								memset(w_adapter->total_tx_data_dropped, 0, sizeof(w_adapter->total_tx_data_dropped));
								memset(w_adapter->total_tx_data_sent, 0, sizeof(w_adapter->total_tx_data_sent));
							
							} else {
								ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("INVALID Q_NUM\n")));
								return -EFAULT;
					
							}
							/* : Reset all the queue stats for now. Individual queue stats for AP/STA needs to be
							 * modified.
							 */
							/* Reset Station queue stats */
#if 0
							w_adapter->total_sta_data_vo_pkt_send = 0;
							w_adapter->total_sta_vo_pkt_freed = 0;
							w_adapter->total_sta_data_vi_pkt_send = 0;
							w_adapter->total_sta_vi_pkt_freed = 0;
							w_adapter->total_sta_data_be_pkt_send = 0;
							w_adapter->total_sta_be_pkt_freed = 0;
							w_adapter->total_sta_data_bk_pkt_send = 0;
							w_adapter->total_sta_bk_pkt_freed = 0;

							/* Reset AP queue stats */
							w_adapter->total_ap_data_vo_pkt_send = 0;
							w_adapter->total_ap_vo_pkt_freed = 0;
							w_adapter->total_ap_data_vi_pkt_send = 0;
							w_adapter->total_ap_vi_pkt_freed = 0;
							w_adapter->total_ap_data_be_pkt_send = 0;
							w_adapter->total_ap_be_pkt_freed = 0;
							w_adapter->total_ap_data_bk_pkt_send = 0;
							w_adapter->total_ap_bk_pkt_freed = 0;

							w_adapter->tx_vo_dropped = 0;
							w_adapter->tx_vi_dropped = 0;
							w_adapter->tx_be_dropped = 0;
							w_adapter->tx_bk_dropped = 0;
#endif
						

			 		w_adapter->buf_semi_full_counter = 0;
			 		w_adapter->buf_full_counter = 0;
					w_adapter->no_buffer_fulls = 0;

#if 0
							switch(qnum)
							{
								case VO_Q:
									w_adapter->total_data_vo_pkt_send = 0;
									w_adapter->total_vo_pkt_freed = 0;
									break;
								case VI_Q:
									w_adapter->total_data_vi_pkt_send = 0;
									w_adapter->total_vi_pkt_freed = 0;
									break;
								case BE_Q:
									w_adapter->total_data_be_pkt_send = 0;
									w_adapter->total_be_pkt_freed = 0;
									break;
								case BK_Q:
									w_adapter->total_data_bk_pkt_send = 0;
									w_adapter->total_bk_pkt_freed = 0;
									break;
								default:
									w_adapter->total_data_vo_pkt_send = 0;
									w_adapter->total_vo_pkt_freed = 0;
									w_adapter->total_data_vi_pkt_send = 0;
									w_adapter->total_vi_pkt_freed = 0;
									w_adapter->total_data_be_pkt_send = 0;
									w_adapter->total_be_pkt_freed = 0;
									w_adapter->total_data_bk_pkt_send = 0;
									w_adapter->total_bk_pkt_freed = 0;
									break;
							}
#endif
						}
					break;
					case AGGR_LIMIT:
						if(copy_from_user(&w_adapter->aggr_limit, wrq->u.data.pointer, wrq->u.data.length))
						{
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT(" Aggr Params Copying Failed\n")));
							return -EINVAL;
						}
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("%s: Aggr params set are tx=%d rx=%d\n"), __func__, w_adapter->aggr_limit.tx_limit, w_adapter->aggr_limit.rx_limit));
						TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
						{ 
							if(vap->iv_opmode == IEEE80211_M_STA){
								break;
							}
						}
						if(vap)
						{
							vap->hal_priv_vap->aggr_rx_limit = w_adapter->aggr_limit.rx_limit;
						}
	 				break;
					case MASTER_READ:
					{
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("performing master read\n")));
						if(w_adapter->devdep_ops->onebox_do_master_ops(w_adapter, wrq->u.data.pointer, ONEBOX_MASTER_READ) != ONEBOX_STATUS_SUCCESS)
						{
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT(" Data Read Failed\n")));
						}
					}
	 				break;
					case MASTER_WRITE:
					{
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("performing master write \n")));
						if(w_adapter->devdep_ops->onebox_do_master_ops(w_adapter, wrq->u.data.pointer, ONEBOX_MASTER_WRITE) != ONEBOX_STATUS_SUCCESS)
						{
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT(" Data Write Failed\n")));
						}
					}
	 				break;
					case TEST_MODE:
					{
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("starting in test mode")));
						memcpy(&test, wrq->u.data.pointer, sizeof(test));
						if(w_adapter->devdep_ops->onebox_send_debug_frame(w_adapter, &test) != ONEBOX_STATUS_SUCCESS)
						{
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT(" Sending Debug frame Failed\n")));
						}
					}
	 				break;
          case SET_COUNTRY:

					if(!w_adapter->sc_nvaps && (d_assets->protocol_enabled & WLAN_PROTOCOL)) {
							status = set_region (ic, value);
							if (status < 0) {
								return -EINVAL;
							}
							w_adapter->net80211_ops->onebox_media_init(ic);
					}	else {
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Vaps Are installed so please ISSUE IOCTL before creating VAPS\n")));
							return -EINVAL;

					}
          break;
					case GET_INFO:
						getinfo = wrq->u.data.pointer;
						if (!strcmp(getinfo->param_name, "country")) {
							memcpy(getinfo->data, ic->ic_regdomain.isocc, 2);
						} else if(!strcmp(getinfo->param_name, "country_code")) {
							*(uint16_t *)(getinfo->data) = ic->ic_regdomain.country;
						} else {
								return -EINVAL;
						}
					break;
					case SET_SCAN_TYPE:
					if(!w_adapter->sc_nvaps && (d_assets->protocol_enabled & WLAN_PROTOCOL) && (w_adapter->fsm_state == FSM_MAC_INIT_DONE)) {
							ic->band_to_scan = value;
							w_adapter->net80211_ops->onebox_media_init(ic);
					}
					else {
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Vaps Are installed so please ISSUE IOCTL before creating VAPS\n")));
					}
					break;
					case SET_WOWLAN_CONFIG:
						TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
						{ 
							if(vap->iv_opmode == IEEE80211_M_STA)
							{
                wowlan_params = &vap->hal_priv_vap->wowlan_params;
                memcpy(wowlan_params, wrq->u.data.pointer, sizeof(struct wowlan_config));
                ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT(" mac addr=%02x:%02x:%02x:%02x:%02x:%02x\n"), wowlan_params->macaddr[0], 
                      wowlan_params->macaddr[1], wowlan_params->macaddr[2], wowlan_params->macaddr[3], wowlan_params->macaddr[4], wowlan_params->macaddr[5]));
                send_wowlan_params(vap, w_adapter);
                found = 1;
              }
            }

            if(!found)
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Vaps Are not installed so please ISSUE IOCTL after creating Station VAP\n")));
					break;

          case SET_EXT_ANT_GAIN:
          {
            ant_gain = wrq->u.data.pointer;
            w_adapter->ant_gain[0] = ant_gain[0];
            w_adapter->ant_gain[1] = ant_gain[1];
						break;
          }
	  case DRV_PARAMS:
	  w_adapter->os_intf_ops->onebox_memcpy(driver_params.mac_addr, w_adapter->mac_addr, IEEE80211_ADDR_LEN );
	  w_adapter->os_intf_ops->onebox_memcpy(driver_params.fw_ver, w_adapter->lmac_ver.ver.info.fw_ver, 8);
	  driver_params.module_type = w_adapter->band_supported;
	  ret_val = copy_to_user(wrq->u.data.pointer, &driver_params, sizeof(driver_params_t));
	  return ret_val;

	  case ADD_APPLE_IE:
	  {
		  TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) {
			  if (vap->iv_opmode == IEEE80211_M_HOSTAP)
				  found = 1;
		  }
		  if (!found) {
			  ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, 
					  (TEXT("No AP VAP found, cannot set \
						APPLE IE:%s\n"),ifr->ifr_name));
			  return ONEBOX_STATUS_FAILURE;
		  }
		  ic->ic_apple_ie.add_apple_ie = true;
		  ic->ic_apple_ie.ie_len = wrq->u.data.length;
		  ic->ic_apple_ie.ie_data = kmalloc(wrq->u.data.length, GFP_ATOMIC);
			if (ic->ic_apple_ie.ie_data == NULL) {
				ic->ic_apple_ie.add_apple_ie = false;
				return -EINVAL;
			}
		  memcpy(ic->ic_apple_ie.ie_data, wrq->u.data.pointer, ic->ic_apple_ie.ie_len);		
	  }
	  break; 
	  case IAP_INIT:
		{
			status = onebox_iap_config(w_adapter, RSI_IAP_INIT, wrq->u.data.pointer);
			return status;
		}
		break;
		case IAP_MFI_CHALLENGE:
		{
			status = onebox_iap_config(w_adapter, RSI_MFI_WRITE_CHALLENGE, wrq->u.data.pointer);
			if (status != ONEBOX_STATUS_SUCCESS) {
				return -EINVAL;
			}

			if (status != ONEBOX_STATUS_SUCCESS) {
				ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Write challenge failed!!!!")));
				return -EINVAL;
			}
			if(w_adapter->os_intf_ops->onebox_wait_event(&(d_assets->iap_event), EVENT_WAIT_FOREVER) < 0 ){
					ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s line no %d : wait event failed\n", __func__, __LINE__));
					return ONEBOX_STATUS_FAILURE;
			}
			w_adapter->os_intf_ops->onebox_reset_event(&d_assets->iap_event);
			memset(wrq->u.data.pointer, 0, 128);
			if (copy_to_user(wrq->u.data.pointer, d_assets->mfi_signature, 128))
				ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("ERROR copying buffer to user\n")));
			return status;
		}
		break;
		case IAP_READ_CERTIFICATE:
		{
			status = onebox_iap_config(w_adapter, RSI_MFI_READ_CERTIFICATE, wrq->u.data.pointer);
			if( w_adapter->os_intf_ops->onebox_wait_event(&(d_assets->iap_event), EVENT_WAIT_FOREVER) < 0 ) {
					ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s line no %d : wait event failed\n", __func__, __LINE__));
					return ONEBOX_STATUS_FAILURE;
			}
			w_adapter->core_ops->onebox_dump(ONEBOX_ZONE_DEBUG, (PUCHAR)d_assets->mfi_certificate, 908);
			if (copy_to_user(wrq->u.data.pointer, d_assets->mfi_certificate, 908))
				ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("ERROR copying buffer to user\n")));

			w_adapter->os_intf_ops->onebox_reset_event(&d_assets->iap_event);
			return status;
		}
		break;
		case CONF_BEACON_RECV:
		{
			TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
			{
					if( vap->iv_opmode == IEEE80211_M_STA )
					{
							status = ONEBOX_STATUS_FAILURE;
							return status;
					}
			}
			status = w_adapter->devdep_ops->conf_beacon_recv(w_adapter,*(uint8 *)wrq->u.data.pointer );
			return status;
		}
		break;
	  default:
	  ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,("No match yet\n"));
	  return -EINVAL;
	  break;
				}
			}
			else if(w_adapter->Driver_Mode == SNIFFER_MODE)
			{
				switch((unsigned char)wrq->u.data.flags)
				{
					case PER_RECEIVE:
						w_adapter->endpoint_params.per_ch_bw = (*(uint8 *)wrq->u.data.pointer & 0x07);
						w_adapter->endpoint_params.enable_11j =(*(uint8 *)wrq->u.data.pointer >> 3 )&0x1;
						w_adapter->recv_channel = (uint8)(wrq->u.data.flags >> 8);
						w_adapter->endpoint_params.channel = w_adapter->recv_channel;
						if(((w_adapter->recv_channel >= 36) && (w_adapter->recv_channel <= 165)) && (!(w_adapter->band_supported)))
						{
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,("5Ghz Band Not supported\n"));
							w_adapter->ch_util_start_flag = 1;
							return -1;
						}
						w_adapter->ch_util_start_flag = 0;
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO,("In %s and %d ch_width %d recv_channel %d \n", __func__, __LINE__, w_adapter->endpoint_params.per_ch_bw, w_adapter->recv_channel));
						w_adapter->devdep_ops->onebox_band_check(w_adapter);
							w_adapter->devdep_ops->onebox_set_channel(w_adapter,w_adapter->recv_channel);
							return ret_val;
							break;
#ifdef CHANNEL_UTILIZATION
						case CH_UTIL_START:
              if (w_adapter->ch_util_start_flag == 0) {
                w_adapter->ch_util_start_flag = *(uint16 *)(wrq->u.data.pointer);

                w_adapter->recv_stop = 0;
                w_adapter->rx_running = 1;
                /*send frame to start receiving stats*/
                //ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("********* %d \n"), w_adapter->ch_util_start_flag));
                if((w_adapter->core_ops->onebox_stats_frame(w_adapter)))
                {
                  ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,("Failed: to send start stats frame\n"));
                  return ONEBOX_STATUS_FAILURE;
                }
                ONEBOX_DEBUG(ONEBOX_ZONE_INFO,("**** Channel Utilization test Started ****\n"));
              }
              else if (w_adapter->ch_util_start_flag == 1)
              {
                ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,("Failed: Card Does not support Band\n"));
                return ONEBOX_STATUS_FAILURE;
              }
              else
              {
                /*wait for event to receive stats from firmware*/
                //ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("waiting for event \n")));
                status = w_adapter->os_intf_ops->onebox_wait_event(&(w_adapter->stats_event), EVENT_WAIT_FOREVER);
								if (status < 0) {
										ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s line no %d : wait event failed %d\n", __func__, __LINE__, status));
										return status;
								}
								w_adapter->os_intf_ops->onebox_reset_event(&(w_adapter->stats_event));
                if(w_adapter->recv_stop) {
                  ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,("Recvd stop Command\n"));
                  break;
                }
                //ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("********* %d %d %d %d %d\n"), w_adapter->sta_info.utilization, w_adapter->sta_info.rssi_utilization, w_adapter->sta_info.tot_bytes, w_adapter->sta_info.rssi_bytes, w_adapter->sta_info.interval_duration));
                ret_val = copy_to_user(wrq->u.data.pointer, &w_adapter->sta_info, sizeof(per_stats));
                return ret_val;
              }
							break;
						case CH_UTIL_STOP:
              w_adapter->ch_util_start_flag = 0; // reset to start channel utilization again
              w_adapter->recv_stop = 1;
              w_adapter->rx_running = 0;
              /* send frame to stop receiving stats*/
              if((w_adapter->core_ops->onebox_stats_frame(w_adapter)))
              {
                ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,("Failed: to send stop stats frame\n"));
              }
              w_adapter->os_intf_ops->onebox_set_event(&(w_adapter->stats_event));
              ONEBOX_DEBUG(ONEBOX_ZONE_INFO,("**** Channel Utilization test Stopped ****\n"));
              break;
#endif
						default:
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Failed: in %d case\n"),(unsigned char)wrq->u.data.flags));
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,("No match yet\n"));
							return -EINVAL;
							break;
					}
			}
			else 
			{
				switch((unsigned char)wrq->u.data.flags)
				{
					case PER_TRANSMIT:
						if(copy_from_user(&w_adapter->endpoint_params, wrq->u.data.pointer, wrq->u.data.length))
						{
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Copying Failed\n")));
							return -EINVAL;
						}
						
						if (w_adapter->endpoint_params.enable)
						{
							if ( w_adapter->endpoint_params.ctry_region == 127 ){
								/* Code for World Domain is 3 */
								country_code = 3;
							} else {
								country_code = w_adapter->endpoint_params.ctry_region;
							}
							if ( ic->ic_regdomain.pad[0] != country_code ) {
								ic->ic_regdomain.pad[0] = country_code;
								w_adapter->net80211_ops->onebox_media_init(ic);
							}
							if (!w_adapter->net80211_ops->onebox_find_channel(ic, w_adapter->endpoint_params.channel, IEEE80211_MODE_AUTO)) {
								ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("Invalid Channel Number\n"));
								return -1;
							}
						}
						if(w_adapter->core_ops->onebox_start_per_tx(w_adapter))
						{
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Invalid arguments issued by user\n")));
							return -EINVAL;
						}
						return 0;
						break;
					case PER_RECEIVE_STOP:
						w_adapter->recv_stop = 1;
						w_adapter->rx_running = 0;
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO,("PER_RECEIVE_STOP\n"));
					case PER_RECEIVE:
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO,("per_ch_bw :%d \n",w_adapter->endpoint_params.per_ch_bw));
						w_adapter->endpoint_params.per_ch_bw = (*(uint8 *)wrq->u.data.pointer & 0x07);
						w_adapter->endpoint_params.enable_11j =(*(uint8 *)wrq->u.data.pointer >> 3 )&0x1;
						w_adapter->recv_channel = (uint8)(wrq->u.data.flags >> 8);
						w_adapter->endpoint_params.channel = w_adapter->recv_channel;
						if (w_adapter->endpoint_params.channel == 0xFF)
						{
							if(w_adapter->devdep_ops->onebox_mgmt_send_bb_reset_req(w_adapter) != ONEBOX_STATUS_SUCCESS) {
								return ONEBOX_STATUS_FAILURE;
							}
						}
						else if (w_adapter->endpoint_params.channel)
						{
							w_adapter->devdep_ops->onebox_band_check(w_adapter);
							w_adapter->fsm_state = FSM_SCAN_CFM;		
							w_adapter->devdep_ops->onebox_set_channel(w_adapter,w_adapter->recv_channel);
							status = w_adapter->os_intf_ops->onebox_wait_event(&(w_adapter->bb_rf_event), EVENT_WAIT_FOREVER);
							if (status < 0) {
									ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s line no %d : wait event failed %d\n", __func__, __LINE__, status));
									return status;
							}
							w_adapter->fsm_state = FSM_MAC_INIT_DONE;
							w_adapter->os_intf_ops->onebox_reset_event(&(w_adapter->bb_rf_event));
						}
						if (!w_adapter->rx_running)
						{
							if(!(w_adapter->core_ops->onebox_stats_frame(w_adapter)))
							{
								w_adapter->rx_running = 1;
								if (w_adapter->recv_stop)
								{
									w_adapter->recv_stop = 0;
									w_adapter->rx_running = 0;
									return ONEBOX_STATUS_SUCCESS;
								}  
							}
						}

						status = w_adapter->os_intf_ops->onebox_wait_event(&(w_adapter->stats_event), EVENT_WAIT_FOREVER);
						if (status < 0) {
								ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s line no %d : wait event failed %d\n", __func__, __LINE__, status));
						}
						w_adapter->os_intf_ops->onebox_reset_event(&(w_adapter->stats_event));
						ret_val = copy_to_user(wrq->u.data.pointer, &w_adapter->sta_info, sizeof(per_stats));
						return ret_val;
						break;
					case PER_PACKET:
						if(copy_from_user(&w_adapter->per_packet, wrq->u.data.pointer, wrq->u.data.length))
						{
							ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Copying PER Packet Failed in %s\n"), __func__));
							return -EINVAL;
						}
						ONEBOX_DEBUG(ONEBOX_ZONE_DEBUG, (TEXT("Copying PER Packet in %s\n"), __func__));
						w_adapter->core_ops->onebox_dump(ONEBOX_ZONE_DEBUG, (PUCHAR)&w_adapter->per_packet.packet, w_adapter->per_packet.length);
						return 0;
						break;
					case ANT_SEL:
						value = ((unsigned short)wrq->u.data.flags >> 8); //endpoint value 
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("ANT_SEL value is : %d \n"),value));
						w_adapter->devdep_ops->onebox_program_ant_sel(w_adapter, value);
						break;
          case SET_EXT_ANT_GAIN:
          //return copy_from_user(w_adapter->ant_gain, wrq->u.data.pointer, wrq->u.data.length);
          ant_gain = wrq->u.data.pointer;
          w_adapter->ant_gain[0] = ant_gain[0];
          w_adapter->ant_gain[1] = ant_gain[1];
						break;
					case SET_ENDPOINT:
						value = ((unsigned short)wrq->u.data.flags >> 8); //endpoint value 
						ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("ENDPOINT type is : %d \n"),value));
						w_adapter->endpoint = value;
#ifdef PROGRAMMING_BBP_TA	
						w_adapter->devdep_ops->onebox_program_bb_rf(w_adapter);
#else
            			if (value == 0)
						{
							w_adapter->endpoint_params.per_ch_bw = 0;
							w_adapter->endpoint_params.channel = 1;
						}
            			else if (value == 1)
						{
							w_adapter->endpoint_params.per_ch_bw = 6;
							w_adapter->endpoint_params.channel = 1;
						}
						else if (value == 2)
						{
							w_adapter->endpoint_params.per_ch_bw = 0;
							w_adapter->endpoint_params.channel = 36;
						}
						else if (value == 3)
						{
							w_adapter->endpoint_params.per_ch_bw = 6;
							w_adapter->endpoint_params.channel = 36;
						}
						w_adapter->devdep_ops->onebox_band_check(w_adapter);
#endif
						break;
          case EEPROM_READ_IOCTL:
          {
            ONEBOX_DEBUG(ONEBOX_ZONE_INFO,("ONEBOX_IOCTL: EEPROM READ  \n"));
	          w_adapter->os_intf_ops->onebox_memset(&w_adapter->eeprom, 0, sizeof(EEPROMRW));
            if(copy_from_user(&w_adapter->eeprom, wrq->u.data.pointer, 
                              (sizeof(EEPROMRW) - sizeof(w_adapter->eeprom.data))))
            {
              ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Copying Failed\n")));
              return -EINVAL;
            }

            status = w_adapter->devdep_ops->onebox_eeprom_rd(w_adapter);
            if (status == ONEBOX_STATUS_SUCCESS)
            {
              status = w_adapter->os_intf_ops->onebox_wait_event(&(w_adapter->bb_rf_event), 10000); 
							if (status < 0) {
									ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s line no %d : wait event failed %d\n", __func__, __LINE__, status));
									return status;
							}
						  ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT(" eeprom length: %d, \n"), w_adapter->eeprom.length));
						  ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT(" eeprom offset: %d, \n"), w_adapter->eeprom.offset));
              ret_val = copy_to_user(wrq->u.data.pointer, &w_adapter->eeprom, sizeof(EEPROMRW));
              w_adapter->os_intf_ops->onebox_reset_event(&(w_adapter->bb_rf_event));
              return ret_val;
            }
            else
            {	
              return ONEBOX_STATUS_FAILURE;
            }
          }
          break;
#if 0
        case EEPROM_WRITE_IOCTL:
          {
            ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,("ONEBOX_IOCTL: EEPROM WRITE  \n"));
#if 1
            if(copy_from_user(&w_adapter->eeprom, wrq->u.data.pointer, wrq->u.data.length))
            {
              ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Copying Failed\n")));
              return -EINVAL;
            }
#endif
            w_adapter->eeprom.length = (wrq->u.data.length - 10);
            ONEBOX_DEBUG(ONEBOX_ZONE_INFO,
                (TEXT("===> Frame to WRITE IN TO EEPROM <===\n")));

            mgmt_frame = (onebox_mac_frame_t *)pkt_buffer;

            w_adapter->os_intf_ops->onebox_memset(mgmt_frame, 0, FRAME_DESC_SZ);

            /* FrameType*/
            mgmt_frame->desc_word[1] = ONEBOX_CPU_TO_LE16(EEPROM_WRITE);
            mgmt_frame->desc_word[0] = (ONEBOX_WIFI_MGMT_Q << 12 | w_adapter->eeprom.length);
            if (!w_adapter->eeprom_erase)
            {
              mgmt_frame->desc_word[2] = ONEBOX_CPU_TO_LE16(BIT(10));
              w_adapter->eeprom_erase = 1;
            }  
            /* Number of bytes to read*/
            mgmt_frame->desc_word[3] = ONEBOX_CPU_TO_LE16(w_adapter->eeprom.length);
            /* Address to read*/
            mgmt_frame->desc_word[4] = ONEBOX_CPU_TO_LE16(w_adapter->eeprom.offset);
            mgmt_frame->desc_word[5] = ONEBOX_CPU_TO_LE16(w_adapter->eeprom.offset >> 16);
          	w_adapter->os_intf_ops->onebox_memcpy(mgmt_frame->u.byte.buf, w_adapter->eeprom.data, w_adapter->eeprom.length);
  
            w_adapter->core_ops->onebox_dump(ONEBOX_ZONE_INFO, (PUCHAR)mgmt_frame, FRAME_DESC_SZ + w_adapter->eeprom.length);
            status = w_adapter->osi_host_intf_ops->onebox_host_intf_write_pkt(w_adapter,
                (uint8 *)mgmt_frame,
                FRAME_DESC_SZ + w_adapter->eeprom.length);
            if (status == ONEBOX_STATUS_SUCCESS)
            {
              w_adapter->os_intf_ops->onebox_wait_event(&(w_adapter->bb_rf_event), 10000); 
              w_adapter->os_intf_ops->onebox_memset(w_adapter->eeprom.data, 0, 480);
              return ONEBOX_STATUS_SUCCESS;
            }
            else
            {	
              return ONEBOX_STATUS_FAILURE;
            }
          }
          break;
#endif
						case PS_REQUEST:
						{
									if((w_adapter->Driver_Mode == RF_EVAL_MODE_ON)) {
										
										send_sleep_req_in_per_mode(w_adapter, wrq->u.data.pointer);

									}
							break;
						}
						case RF_PWR_MODE:
						{
							memcpy(&w_adapter->rf_pwr_mode, wrq->u.data.pointer, sizeof(w_adapter->rf_pwr_mode));	
							w_adapter->rf_power_mode_change = 1;
							ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("Setting RF PWR MODE %04x\n"), w_adapter->rf_pwr_mode));
							break;
						}

					default:
						ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,("No match yet\n"));
						return -EINVAL;
						break;
				}
			}
			return ret_val;
		}
		break;
		case ONEBOX_SET_BB_RF:
		{
//			if(w_adapter->Driver_Mode == RF_EVAL_MODE_ON)
			{
				if(copy_from_user(&w_adapter->bb_rf_params.Data[0], wrq->u.data.pointer, (sizeof(w_adapter->bb_rf_params))))
				{
					ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Copying Failed\n")));
					return -EINVAL;
				}
				if(w_adapter->devdep_ops->onebox_set_bb_rf_values(w_adapter, wrq))
				{
					ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Invalid arguments issued by user\n")));
					return -EINVAL;
				}
				return ONEBOX_STATUS_SUCCESS;
			}	
		}
		break;
		case ONEBOX_SET_CW_MODE:
		{
			if(w_adapter->Driver_Mode == RF_EVAL_MODE_ON)
      		{
        		channel = (uint8 )wrq->u.data.flags;
				if (!channel)
					channel = 1;
        		if(channel <= 14)
        		{
          			set_band = BAND_2_4GHZ;
					w_adapter->endpoint = 0;
       			}
        		else if((channel >= 36) && (channel <= 165))
        		{
          			set_band = BAND_5GHZ;
					w_adapter->endpoint = 2;
        		}
        		if (w_adapter->operating_band != set_band)
        		{
          			w_adapter->operating_band = set_band;
          			w_adapter->rf_reset = 1;
					w_adapter->devdep_ops->onebox_program_bb_rf(w_adapter);
        		}
				ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("ONEBOX_IOCTL:  channel is %d \n"),channel));
				w_adapter->fsm_state = FSM_SCAN_CFM;		
        			w_adapter->devdep_ops->onebox_set_channel(w_adapter,channel);
				status = w_adapter->os_intf_ops->onebox_wait_event(&(w_adapter->bb_rf_event), EVENT_WAIT_FOREVER);
				if (status < 0) {
						ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s line no %d : wait event failed %d\n", __func__, __LINE__, status));
						return status;
				}
				w_adapter->fsm_state = FSM_MAC_INIT_DONE;
				w_adapter->os_intf_ops->onebox_reset_event(&(w_adapter->bb_rf_event));

				channel = (unsigned int)wrq->u.data.flags; //cw_type & subtype info
				w_adapter->cw_type	=	(channel & 0x0f00) >> 8 ;
				w_adapter->cw_sub_type	=	(channel & 0xf000) >> 12 ;
				ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("ONEBOX_IOCTL:  SET_CW_MODE , cw_type:%d cw_mode:%d \n"),w_adapter->cw_sub_type,w_adapter->cw_type));
				if(w_adapter->devdep_ops->onebox_cw_mode(w_adapter, w_adapter->cw_sub_type))
				{
				  ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("Invalid arguments issued by user\n")));
				  return -EINVAL;
				}
				break;
				return ONEBOX_STATUS_SUCCESS;
		      }	
		}
		break;
		default:
		{
			if (priv == 0) /* req is a standard ioctl */ 
			{
				ONEBOX_DEBUG(ONEBOX_ZONE_INFO, ("Standard ioctl: %d\n", index));
			} 
			else /* Ignore it, Bad ioctl */ 
			{
				ONEBOX_DEBUG(ONEBOX_ZONE_INFO, ("Unrecognised ioctl: %d\n", cmd));
			}
			return -EFAULT;
		}
	}
	return ret_val;
}


