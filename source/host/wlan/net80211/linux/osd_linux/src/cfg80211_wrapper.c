
/*
 * @file
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
 * This file contians the code specific to Litefi-AP architecture.
 */

#ifdef ONEBOX_CONFIG_CFG80211

#include <net/cfg80211.h>
#include <linux/rtnetlink.h>
#include <linux/version.h>
#include <linux/hardirq.h>
#include "linux/wireless.h"
#include "cfg80211_wrapper.h"
#include "cfg80211_ioctl.h"

#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0))
/**
 *  struct ieee80211_iface_combination - possible interface combination
 *   @limits: limits for the given interface types
 *   @n_limits: number of limitations
 *   @num_different_channels: can use up to this many different channels
 *   @max_interfaces: maximum number of interfaces in total allowed in this group
 *   @beacon_int_infra_match: In this combination, the beacon intervals
 *   between infrastructure and AP types must match. This is required
 *   only in special cases.
 *   @radar_detect_widths: bitmap of channel widths supported for radar detection
 *   @radar_detect_regions: bitmap of regions supported for radar detection
 *   With this structure Driver  can describe which interface combinations  it supports
 *   concurrently
 *   Allow  #STA <=1 and #AP<=3, channels=1, max interfaces 4 total	
 *   
 */
static const struct ieee80211_iface_limit obm_ap_sta_limits[] = {

	{
		.max = 4, .types = BIT(NL80211_IFTYPE_AP),
	},
	{
		.max = 1, .types = BIT(NL80211_IFTYPE_STATION),
	},
#if 0
	{
		.max = 1, .types = 	BIT(NL80211_IFTYPE_P2P_GO) |
			BIT(NL80211_IFTYPE_P2P_CLIENT),
	},
#endif
};

static const struct ieee80211_iface_combination obm_iface_comb_ap_sta = {
	.limits = obm_ap_sta_limits, 	
	.num_different_channels = 1,	
	.n_limits = ARRAY_SIZE(obm_ap_sta_limits),	
	.max_interfaces = 4, 
	.beacon_int_infra_match = true ,
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0))
	.radar_detect_widths =	BIT(NL80211_CHAN_NO_HT) |
					BIT(NL80211_CHAN_HT20) | BIT(NL80211_CHAN_HT40MINUS) |
					BIT(NL80211_CHAN_HT40PLUS),
#endif
	

};
#endif

/**
 * This function sets the wiphy parameters if changed. 
 *
 * @param  Pointer to wiphy structure.  
 * @param  Value that indicates which wiphy parameter changed.  
 * @return ONEBOX_STATUS_SUCCESS on success else negative number on failure. 
 */
ONEBOX_STATUS 
onebox_cfg80211_set_wiphy_params(struct wiphy *wiphy, uint32_t changed)
{
	struct iw_param rts;
	struct iw_param frag;
 	struct cfg80211_priv *cfg_priv = wiphy_priv(wiphy);
	if (changed & WIPHY_PARAM_RTS_THRESHOLD)
	{
		rts.value = wiphy->rts_threshold;
		rts.disabled = 0;
		if(onebox_siwrts(cfg_priv->ic, &rts) < 0)
		{
			return ONEBOX_STATUS_FAILURE;
		}
		else
		{
			return ONEBOX_STATUS_SUCCESS;
		}
	}

	if (changed & WIPHY_PARAM_FRAG_THRESHOLD)
	{
		frag.value = wiphy->frag_threshold;
		frag.disabled = 0;
		if(onebox_siwfrag(cfg_priv->ic, &frag) < 0)
		{
			return ONEBOX_STATUS_FAILURE;
		}
		else
		{
			return ONEBOX_STATUS_SUCCESS;
		}
	}
	return ONEBOX_STATUS_SUCCESS;
}

#if(LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 38))
ONEBOX_STATUS onebox_cfg80211_add_key(struct wiphy *wiphy,struct net_device *ndev,
                                      uint8_t index, const uint8_t  *mac_addr, struct key_params *params)
#else
ONEBOX_STATUS onebox_cfg80211_add_key(struct wiphy *wiphy,struct net_device *ndev,
                                      uint8_t index, bool pairwise, const uint8_t  *mac_addr, struct key_params *params)
#endif
{

	if(onebox_add_key(ndev, index, mac_addr, (struct ieee80211_key_params *)params) < 0)
	{
		return ONEBOX_STATUS_FAILURE;
	}
	else
	{
		return ONEBOX_STATUS_SUCCESS;
	}
}

/**
 * This function deletes the key of particular key_index. 
 *
 * @param  Pointer to wiphy structure.  
 * @param  Pointer to network device structure.  
 * @param  Key Index to be deleted.  
 * @param  Value to identify pairwise or group key.  
 * @param  Pointer to the MAC address of the station to delete keys.  
 * @return GANGES_STATUS_SUCCESS on success else negative number on failure. 
 */

#if(LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 38))
ONEBOX_STATUS 
onebox_cfg80211_del_key(struct wiphy *wiphy, struct net_device *ndev, uint8_t index,
                        const uint8_t *mac_addr )
#else
ONEBOX_STATUS 
onebox_cfg80211_del_key(struct wiphy *wiphy, struct net_device *ndev, uint8_t index,
                        bool pairwise, const uint8_t *mac_addr )
#endif
{
	if(onebox_delete_key(ndev, mac_addr, index) < 0)
	{
		return ONEBOX_STATUS_FAILURE;
	}
	else
	{
		return ONEBOX_STATUS_SUCCESS;	
	}
}

/**
 * This function sets the various connection params for infra mode.
 *
 * @param  Pointer to wiphy structure.  
 * @param  Pointer to network device structure.  
 * @param  Mode to set Infrastructure/IBSS.  
 * @param  Pointer to flags.  
 * @param  Pointer to interface parameters.  
 * @return ONEBOX_STATUS_SUCCESS on success else negative number on failure. 
 */
ONEBOX_STATUS 
onebox_cfg80211_connect(struct wiphy *wiphy, struct net_device *ndev, struct cfg80211_connect_params *params)
{
	cfg80211_sta_connect_params connect_params;

	connect_params.auth_type = params->auth_type;
	connect_params.ie = params->ie;
	connect_params.ie_len = params->ie_len;
	connect_params.ssid = params->ssid;
	connect_params.ssid_len = params->ssid_len;
	connect_params.bssid = (u8*)params->bssid;
	connect_params.privacy = params->privacy;


	if(params->key)
	{
		if(onebox_wep_key(ndev, params->key_idx, params->bssid, params->key_len, params->key) < 0)
		{
			return ONEBOX_STATUS_FAILURE;
		}
	}

	if(cfg80211_connect_res(ndev, &connect_params) < 0)
	{
		return ONEBOX_STATUS_FAILURE;
	}
	else
	{
		return ONEBOX_STATUS_SUCCESS;
	}
}

/**
 * This function registers our device as a wiphy device and registers the 
 * initial wiphy parameters. We allocate space for wiphy parameters. 
 *
 * @param  Pointer to our device(pfunc).  
 * @return Pointer to the wireless dev structure obtained after registration. 
 */
struct wireless_dev* onebox_register_wireless_dev(struct net_device *ndev, struct cfg80211_priv *cfg_priv, int opmode)
{
	struct wireless_dev *wdev = NULL;
	
	if(!ndev) {
		return NULL;
	}

	if(!cfg_priv) {
		return NULL;
	
	}
	wdev = kzalloc(sizeof(struct wireless_dev), GFP_KERNEL);
	if (!wdev)
	{
		return NULL;
	}

	if (!wdev)
	{
		return NULL;
	}
	ndev->ieee80211_ptr = wdev;
	wdev->wiphy = cfg_priv->wiphy;
	SET_NETDEV_DEV(ndev, wiphy_dev(cfg_priv->wiphy));
	wdev->netdev = ndev;

	if(opmode == 1)
	{
      		wdev->iftype = NL80211_IFTYPE_STATION;
	} else if(opmode ==4 ){
      		wdev->iftype = NL80211_IFTYPE_AP;
	} else if(opmode == 9){
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 7, 0))
      		wdev->iftype = NL80211_IFTYPE_P2P_DEVICE;
#else
      		wdev->iftype = NL80211_IFTYPE_UNSPECIFIED;
#endif
  } else
		wdev->iftype = NL80211_IFTYPE_UNSPECIFIED;

	return wdev;
}

void onebox_update_channel_flags(struct wiphy *wiphy, struct cfg80211_priv *cfg_priv)
{
	struct ieee80211_supported_band *sband;
	struct ieee80211_channel *cfg_ch;
	uint32_t ii = 0, jj=0;

	for (ii= 0; ii < 2; ii++) {
		sband = (struct ieee80211_supported_band *)wiphy->bands[ii];
		if (sband) {
			for (jj = 0; jj < sband->n_channels; jj++) {
				cfg_ch = &sband->channels[jj];
				if (cfg_ch) {
					/* : max allowed tx power is present in ch->max_power. To be taken care of when TPC is being implemented */
					apply_chan_flags(cfg_priv->ic, cfg_ch->center_freq, cfg_ch->flags, cfg_ch->max_power);
				}
			}
		}
	}
}

/**
 * This function triggers the scan. 
 *
 * @param  Pointer to wiphy structure.  
 * @param  Pointer to network device structure.  
 * @param  Pointer to the cfg80211_scan_request structure
 *         that has the scan parameters.  
 * @return ONEBOX_STATUS_SUCCESS on success else negative number on failure. 
 */
ONEBOX_STATUS 
#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
onebox_cfg80211_scan(struct wiphy *wiphy, struct net_device *ndev,struct cfg80211_scan_request *request)
#else
onebox_cfg80211_scan(struct wiphy *wiphy, struct cfg80211_scan_request *request)
#endif
{
	uint16_t *channels= NULL;
	uint8_t n_channels = 0;
  //struct cfg80211_priv *cfg_priv = wiphy_priv(wiphy);
	ONEBOX_STATUS status = ONEBOX_STATUS_SUCCESS;	
	struct net_device *netdev;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
	netdev = ndev;
#else
	netdev = request->wdev->netdev;
#endif

	if(request->n_channels > 0  && (request->n_channels <= IEEE80211_MAX_FREQS_ALLOWED ))
	{

		uint8_t i;
		n_channels = request->n_channels;
        	channels = kzalloc(n_channels * sizeof(uint16_t), GFP_KERNEL);
		if (channels == NULL) 
		{
            		n_channels = 0;
        	}
        	for (i = 0; i < n_channels; i++)
		{
			channels[i] =  request->channels[i]->center_freq;
		}
	}
	else
	{
		request->n_channels = 0;

	}
	/* Update channel flags */
	onebox_update_channel_flags(wiphy, wiphy_priv(wiphy));
	if(cfg80211_scan(request, netdev, n_channels, channels,  request->ie, request->ie_len, request->n_ssids, request->ssids))
	{
		status = ONEBOX_STATUS_FAILURE;
	}
	else
	{
		/* No code here */
	}
	if(channels)
	{
		kfree(channels);
	}
	return status;
}	

#if(LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38))
ONEBOX_STATUS 
onebox_cfg80211_set_txpower(struct wiphy *wiphy, enum nl80211_tx_power_setting type, int dbm)
{
	struct cfg80211_priv *cfg_priv;
	
	cfg_priv = (struct cfg80211_priv *)(wiphy_priv(wiphy));
	dbm = (dbm / 100); //convert mbm to dbm
	if (type == NL80211_TX_POWER_FIXED)
	{
		if (dbm < 0)
		{
			dbm = 0;
		}
		else if (dbm > 20) /* Check the max */
		{
			dbm = 20;
		}
		else
		{
      		/* No code here */
		} /* End if <condition> */
	}
	else 
	{
    	/* Automatic adjustment */
    	dbm = 0xff;
  	} /* End if <condition> */
	return tx_power(cfg_priv->ic, dbm);
}
#endif

ONEBOX_STATUS 
onebox_cfg80211_set_default_key(struct wiphy *wiphy, struct net_device *ndev, uint8_t index
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38))
		, bool unicast, bool multicast
#endif
		)
{
	return ONEBOX_STATUS_SUCCESS;
}

/**
 * This function is a callback from cfg80211 for disconnecting the
 * Station Mode Interface from the connected AP.
 */
ONEBOX_STATUS 
onebox_cfg80211_disconnect(struct wiphy *wiphy, struct net_device *ndev, uint16_t reason_code)
{
	if(cfg80211_disconnect(ndev, reason_code) < 0)
	{
		return ONEBOX_STATUS_FAILURE;
	}
	else
	{
		return ONEBOX_STATUS_SUCCESS;
	}
}

ONEBOX_STATUS
onebox_cfg80211_get_station(struct wiphy *wiphy, struct net_device *ndev, 
#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 16, 0))
		uint8_t *mac, 
#else
		const uint8_t *mac, 
#endif
		struct station_info *info)
{
	return ONEBOX_STATUS_SUCCESS;
}

void cfg80211_wrapper_detach(struct wiphy *wiphy)
{
	if(!wiphy)
	{
		return;
	}
	wiphy_unregister(wiphy);
	wiphy_free(wiphy);
	return;
}


#define SIGNAL_LEVEL 24
int scan_results_sup(struct wireless_dev *wdev, struct ieee80211_scan_h *hscan)
{
	struct wiphy *wiphy = wdev->wiphy;

	struct ieee80211_mgmt *mgmt = NULL;
	struct cfg80211_bss *bss = NULL;
	char *temp;
	BEACON_PROBE_FORMAT *frame;
	int frame_len = 0;
	uint8_t band;
	int freq;
	struct ieee80211_channel *channel = NULL;
	int signal_level;

	nl80211_mem_alloc((void **)&mgmt, sizeof(struct ieee80211_mgmt) + 512, GFP_ATOMIC);
	nl80211_memcpy(mgmt->bssid, hscan->se_bssid, IEEE80211_ADDR_LEN);
	mgmt->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT); 
	frame = (BEACON_PROBE_FORMAT *)(&mgmt->u.beacon); 
	temp = (unsigned char *)&frame->timestamp[0];

	memset(temp, 0, 8);
	frame->beacon_intvl = cpu_to_le16(hscan->se_intval);
	frame->capability   = cpu_to_le16(hscan->se_capinfo);
	temp = frame->variable;
	/** Added the below 3 lines code to give ssid field in the data ie **/
	/** Made this change for Hidden mode AP's **/
	if ((hscan->data[0] == WLAN_EID_SSID && hscan->data[1] == 0))
	{
		*temp++ = WLAN_EID_SSID;
		*temp++ = hscan->se_ssid[1];
		memcpy(temp, &hscan->se_ssid[2], hscan->se_ssid[1]);
		temp = temp + hscan->se_ssid[1];
		frame_len = hscan->se_ssid[1];
		nl80211_memcpy(temp, hscan->data, hscan->len );
		hscan->len -= 2;
		hscan->data += 2 ;
	}
	nl80211_memcpy(temp, hscan->data, hscan->len );
	frame_len += offsetof(struct ieee80211_mgmt, u.beacon.variable);
	frame_len += hscan->len;
	if (hscan->ic_ieee <= 14)
		band = IEEE80211_BAND_2GHZ;
	else
		band = IEEE80211_BAND_5GHZ;

#if(LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 38))
	freq = ieee80211_channel_to_frequency(hscan->ic_ieee);
#else
	freq = ieee80211_channel_to_frequency(hscan->ic_ieee, band);
#endif

	channel = ieee80211_get_channel(wiphy, freq);

	if (!wiphy || !channel || !mgmt || 
			frame_len < offsetof(struct ieee80211_mgmt, u.beacon.variable))
	{
		kfree(mgmt);  
		return ONEBOX_STATUS_FAILURE;
	}

	if (hscan->se_rssi <= 50) {
		signal_level = 100;
	} else if(hscan->se_rssi >= 90) {
		signal_level = 0;
	} else {
		signal_level = abs(216 - ((SIGNAL_LEVEL*hscan->se_rssi)/10)) ;
	}

	bss = cfg80211_inform_bss_frame(wiphy, channel, mgmt,
			cpu_to_le16(frame_len),
			signal_level*100, GFP_ATOMIC);

	if (!bss)
	{
		kfree(mgmt);
		return ONEBOX_STATUS_FAILURE;
	}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 9, 0))
	cfg80211_put_bss(bss);/*this is needed to update bss list */
#else
	cfg80211_put_bss(wiphy,bss);/*this is needed to update bss list */
#endif
	kfree(mgmt);
	return ONEBOX_STATUS_SUCCESS;
}

#if((LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) && (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 5, 7)))
void obm_inform_mgmt_cfg80211(struct net_device *dev, uint8_t *buf, uint16_t len, uint16_t recv_freq, int32_t sig_dbm)
#else
void obm_inform_mgmt_cfg80211(struct wireless_dev *wdev, uint8_t *buf, uint16_t len, uint16_t recv_freq, int32_t sig_dbm)
#endif
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	uint32_t status;

	status = cfg80211_rx_mgmt(
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 5, 7))
			dev,
#else
			wdev, 
#endif
			recv_freq, 
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0))  
			sig_dbm,
#endif
			buf, len, 
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0))  
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(3, 12, 0)) &&\
      (LINUX_VERSION_CODE < KERNEL_VERSION(3, 18, 0))) //bet 3.12 and 3.17
			0,
#endif  
			GFP_KERNEL
#else
			0
#endif
			);
#endif
	return ;
}

int scan_done(void *scan_req, bool scan_status)
{

	if(scan_req == NULL)
	{
		dump_stack();
		return 0;
	}

	cfg80211_scan_done((struct cfg80211_scan_request *)scan_req, scan_status);

	return ONEBOX_STATUS_SUCCESS;
}

void nl80211_mem_alloc(void **ptr, unsigned short len, unsigned short flags)
{
	*ptr = kmalloc(len, flags);
}

void* nl80211_memcpy(void *to, const void *from, int len)
{
	return memcpy(to,from,len);
}

void notify_sta_connect_to_cfg80211(struct wireless_dev *wdev, uint8_t mac[6])
{
	cfg80211_connect_result(wdev->netdev, mac, NULL, 0, NULL, 0, 0, GFP_ATOMIC);
}

void notify_sta_disconnect_to_cfg80211(struct wireless_dev *wdev)
{

	cfg80211_disconnected(wdev->netdev, 0, NULL, 0, 
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 2, 0))
			0,
#endif
			GFP_ATOMIC);
}

#define CFG80211_OPMODE_NET80211(a) ( \
        (a == NL80211_IFTYPE_STATION) ? 1 : \
        (a == NL80211_IFTYPE_AP ) ? 4: \
       -1 )
        
/**
 * This function is a callback from cfg80211 for creating a 
 * virtual interface.
 */
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)) &&\
		(LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 38))) //bet 3.12 and 3.17
int
#elif (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 5, 7))
struct net_device*
#else
struct wireless_dev*
#endif
obm_add_virtual_intf(struct wiphy *wiphy,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0))
					      char *name,
#else
					      const char *name,
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
					      unsigned char name_assign_type,	
#endif
					      enum nl80211_iftype type,
					      u32 *flags,
					      struct vif_params *params)
{
	int vap_id;
	struct cfg80211_priv *cfg_priv = (struct cfg80211_priv *)(wiphy_priv(wiphy));
	uint8_t opmode;
	struct wireless_dev *wdev;

	if (!cfg_priv) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 38))
		return -EINVAL;
#else
		return ERR_PTR(-ENOMEM);
#endif	
	}

	//opmode = (CFG80211_OPMODE_NET80211(type));
	if (type == NL80211_IFTYPE_STATION) {
		opmode = 1;
	}	else if (type == NL80211_IFTYPE_AP) {
		opmode = 4;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 7, 0))
	}	else if (type == NL80211_IFTYPE_P2P_DEVICE) {
		opmode = 9;
#endif
	} else {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 38))
		return -EINVAL;
#else
		return NULL;
#endif	
	}


	vap_id = obm_cfg80211_add_intf(wiphy, name, opmode, flags, wiphy->perm_addr, cfg_priv);	

	/* XXX: Can't we use ieee80211_iface_limit , max_interfaces variable here */
	if (vap_id > 4) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 38))
		return -EINVAL;
#else
		return ERR_PTR(-ENOMEM);
#endif	
	}
	wdev = cfg_priv->wdev[vap_id];
	wdev->iftype = type;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 38))
	return ONEBOX_STATUS_SUCCESS ;
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
	return wdev->netdev;
#else
	return cfg_priv->wdev[vap_id];
#endif
}

/**
 * This function is a callback from cfg80211 to delete the virtual
 * interface.
 *
 */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 6, 0))
int obm_delete_virtual_intf(struct wiphy *wiphy, struct wireless_dev *wdev)
#else
int obm_delete_virtual_intf(struct wiphy *wiphy, struct net_device *ndev)
#endif
{
	struct cfg80211_priv *cfg_priv = (struct cfg80211_priv *)(wiphy_priv(wiphy));

	obm_cfg80211_del_intf(cfg_priv, 
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 6, 0))
	wdev->netdev
#else
	ndev
#endif
	);
	return 0;	
}


#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
void inform_cfg80211_node_join(struct net_device *ndev, 
                       uint8_t *mac_address, 
                       uint8_t *assoc_req_ie,
                       uint8_t assoc_ie_len) 
{
  struct station_info sinfo = {0};

#if(LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0))
  sinfo.filled = STATION_INFO_ASSOC_REQ_IES;
#endif
  sinfo.assoc_req_ies = assoc_req_ie;
  sinfo.assoc_req_ies_len = assoc_ie_len;

  cfg80211_new_sta(ndev, mac_address,
                    &sinfo, GFP_KERNEL);
  return ;
}

void inform_cfg80211_node_leave(struct net_device *ndev, 
                         uint8_t *mac_address) 
{

  cfg80211_del_sta(ndev, mac_address,
                    GFP_KERNEL);
  return ;
}

/** cfg80211 operation handler for start_ap.
 ** Function sets beacon period, DTIM period, SSID and security into
 ** AP config structure.
 ** AP is configured with these settings and BSS is started.
 */
static int 
onebox_cfg80211_start_ap(struct wiphy *wiphy,
		struct net_device *ndev,
		struct cfg80211_ap_settings *params)
{
	struct obm_cfg80211_ap_params ap_params;
	memset(&ap_params, 0, sizeof(struct obm_cfg80211_ap_params));

	nl80211_memcpy(&ap_params.head, &params->beacon, sizeof(struct cfg80211_beacon_data));
	ap_params.ssid = params->ssid;
	ap_params.ssid_len = params->ssid_len;
	ap_params.beacon_interval = params->beacon_interval;
	ap_params.dtim_period = params->dtim_period;
	ap_params.privacy = params->privacy;
	ap_params.auth_mode = params->auth_type;
	ap_params.hide_ssid =  params->hidden_ssid;

	nl80211_memcpy(&ap_params.security, &params->crypto, sizeof(struct cfg80211_crypto_settings));

#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
#if((LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))&& \
		(LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))) 
	ap_params.channel = ieee80211_frequency_to_channel(params->channel->center_freq);

	
#else
	ap_params.channel = ieee80211_frequency_to_channel(params->chandef.chan->center_freq);

#endif
#endif

#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))

  switch( params->chandef.width )
  {
    case NL80211_CHAN_WIDTH_20_NOHT:
    case NL80211_CHAN_WIDTH_20:
      ap_params.channel_width = BAND_WIDTH_20MHZ;
      break;
    case NL80211_CHAN_WIDTH_40:
      ap_params.channel_width = BAND_WIDTH_40MHZ;
      break;
    default:
      ap_params.channel_width = BAND_WIDTH_40MHZ;
  }
#else
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
  switch( params->channel_type )
  {
    case NL80211_CHAN_NO_HT:
    case NL80211_CHAN_HT20:
      ap_params.channel_width = BAND_WIDTH_20MHZ;
      break;
    case NL80211_CHAN_HT40MINUS:
    case NL80211_CHAN_HT40PLUS:
      ap_params.channel_width = BAND_WIDTH_40MHZ;
      break;
    default:
      ap_params.channel_width = BAND_WIDTH_40MHZ;
  }

#endif
#endif

	return obm_cfg_start_ap(ndev, &ap_params);
}

/* cfg80211 operation handler for stop ap.
 * Function stops BSS running at uAP interface.
 */
static int onebox_cfg80211_stop_ap(struct wiphy *wiphy, struct net_device *dev)
{

	return 0;
}

/* cfg80211 operation handler for change_beacon.
 * Function retrieves and sets modified management IEs to FW.
 */
static int onebox_cfg80211_change_beacon(struct wiphy *wiphy,
		struct net_device *ndev,
		struct cfg80211_beacon_data *info)
{
	struct obm_cfg_ap_beacon_ie ap_beacon_ie;

	nl80211_memcpy(&ap_beacon_ie, info, sizeof(struct cfg80211_beacon_data));
	return obm_cfg_change_beacon(ndev, &ap_beacon_ie );
}
#endif

#define	IEEE80211_MLME_ASSOC		1	/* associate station */
#define	IEEE80211_MLME_DISASSOC		2	/* disassociate station */
#define	IEEE80211_MLME_DEAUTH		3	/* deauthenticate station */


/**
 * This function is a callback from cfg80211 for disassociating a
 * Station which is connected to an AP mode Interface.
 */
int onebox_disassoc_station(struct wiphy *wiphy, struct net_device *ndev,
		struct cfg80211_disassoc_request *req
#if(LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
		,void *cookie
#endif
		)
{
	return obm_delete_connected_sta(ndev, req->bss->bssid, IEEE80211_MLME_DISASSOC, req->reason_code);
}

int	onebox_cfg80211_set_channel(struct wiphy *wiphy, struct net_device *ndev,
		struct ieee80211_channel *chan,
		enum nl80211_channel_type channel_type) 
{
	return onebox_set_channel(ndev, ieee80211_frequency_to_channel(chan->center_freq));
}


/**
 * This function is a callback from cfg80211 for deauthenticating a
 * Station which is connected to an AP mode Interface.
 */
int onebox_deauth_station(struct wiphy *wiphy, struct net_device *ndev,
                           struct cfg80211_deauth_request *req
#if(LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
													 ,void *cookie
#endif
													 ) 
{

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
		return obm_delete_connected_sta(ndev, (uint8_t *)req->bss->bssid, IEEE80211_MLME_DEAUTH , req->reason_code);
#else
		return obm_delete_connected_sta(ndev, (uint8_t *)req->bssid, IEEE80211_MLME_DEAUTH , req->reason_code);
#endif
}

void notify_mic_failure_to_cfg80211(struct net_device *ndev, 
                                    uint8_t *mac_addr, 
																		uint8_t key_type,
																		int32_t key_id)
{
		cfg80211_michael_mic_failure(ndev,
		                             mac_addr,
		                             key_type,
																 key_id,
																 NULL, GFP_KERNEL);

}

/**
 * This function is a callback from cfg80211 for disconnecting the 
 * Station which is connected to AP mode Interface.
 */
int 
onebox_del_station(struct wiphy *wiphy, struct net_device *ndev,
#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 16, 0))
								u8 *mac 
#elif ((LINUX_VERSION_CODE >= KERNEL_VERSION(3,16 , 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(3, 19, 0)))
								const u8 *mac 
#else
								struct station_del_parameters *params
#endif
								)
{

	uint32_t reason_code ;
#if((LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)))
	uint8_t *mac = (uint8_t *)params->mac;
	reason_code = params->reason_code;
#else
	reason_code = 2;
#endif

	if(mac) {
		return obm_delete_connected_sta(ndev, mac, IEEE80211_MLME_DEAUTH, 2);
	}
	return -1;
}


int onebox_mgmt_tx(struct wiphy *wiphy, 
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0))
		struct net_device *ndev,
#else
		struct wireless_dev *wdev,
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
		struct ieee80211_channel *chan, bool offchan,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 8, 0))
		enum nl80211_channel_type channel_type, 
		bool channel_type_valid, 
#endif
		unsigned int wait, const u8 *buf, size_t len,
		bool no_cck, bool dont_wait_for_ack, u64 *cookie
#else
		struct cfg80211_mgmt_tx_params *params, 
		u64 *cookie

#endif
		)
{
  return 0;
}
    
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0))
int onebox_set_mac_acl( struct wiphy *wiphy, struct net_device *dev,
		const struct cfg80211_acl_data *params)
{

	return obm_set_mac_acl(dev, params->acl_policy, params->n_acl_entries,
		   	(acl_mac_address *)params->mac_addrs );
}

void obm_cfg80211_radar_notify( struct net_device *ndev, 
		enum nl80211_radar_event type,
		struct obm_cfg80211_ap_params *ap_params)
{

	struct wireless_dev	*wdev = ndev->ieee80211_ptr;
	struct wiphy *wiphy = NULL;
	struct cfg80211_chan_def chandef;
	struct ieee80211_channel chan;
	gfp_t gfp;

	wiphy = wdev->wiphy;

	/*Initialize the chandef from ap_params*/
	chan.band = ap_params->chan.band ;
	chan.center_freq = ap_params->chan.center_freq ;
	chan.flags = ap_params->chan.flags ;
	chan.dfs_state = ap_params->chan.dfs_state ;
	chan.dfs_state_entered =ap_params->chan.dfs_state_entered ;

	chandef.width = ap_params->chan.width;
	chandef.center_freq1 = ap_params->chan.center_freq_1;
	chandef.center_freq2 = ap_params->chan.center_freq_2;

	chandef.chan = &chan;

	if(in_softirq()){
		gfp = GFP_ATOMIC;
	}else{
		gfp = GFP_KERNEL;
	}

	if (!(type == NL80211_RADAR_DETECTED)) {
		/* Indicate CAC COMPLETE event to cfg80211 */
		cfg80211_cac_event(ndev, 
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
		&chandef,
#endif
		type, 
		gfp);
	} else {
		if ( wdev->cac_started ) {
			/** 
			 * Indicate CAC ABORTED when a Radar has been detected 
			 * during CAC time and then inidicate radar event to cfg80211.
			 */
			cfg80211_cac_event(ndev, 
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
					&chandef,
#endif
					NL80211_RADAR_CAC_ABORTED,
			gfp);
		}
		cfg80211_radar_event( wiphy, &chandef, gfp);
	}
	return;
}

int onebox_start_radar_detection( struct wiphy *wiphy, 
		struct net_device *dev, struct cfg80211_chan_def *chandef
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0))
		,u32 cac_time_ms
#endif
		)
{
	struct obm_cfg80211_ap_params ap_params;

	ap_params.channel = ieee80211_frequency_to_channel(chandef->chan->center_freq);

	switch( chandef->width )
	{
		case NL80211_CHAN_WIDTH_20_NOHT:
		case NL80211_CHAN_WIDTH_20:
			ap_params.channel_width = BAND_WIDTH_20MHZ;
			break;
		case NL80211_CHAN_WIDTH_40:
			ap_params.channel_width = BAND_WIDTH_40MHZ;
			break;
		default:
			ap_params.channel_width = BAND_WIDTH_40MHZ;
	}


	ap_params.chan.width = chandef->width;
	ap_params.chan.center_freq_1 = chandef->center_freq1;
	ap_params.chan.center_freq_2 = chandef->center_freq2;

	ap_params.chan.band = chandef->chan->band;
	ap_params.chan.center_freq = chandef->chan->center_freq;
	ap_params.chan.flags = chandef->chan->flags;
	ap_params.chan.dfs_state = chandef->chan->dfs_state;
	ap_params.chan.dfs_state_entered = chandef->chan->dfs_state_entered;


	obm_start_dfs_cac(dev, &ap_params);
	return ONEBOX_STATUS_SUCCESS;
}
#endif


#if 0
/*
 * CFG802.11 operation handler to remain on channel.
 */
static int
obm_cfg80211_remain_on_channel(struct wiphy *wiphy,
				   struct wireless_dev *wdev,
				   struct ieee80211_channel *chan,
				   unsigned int duration, u64 *cookie)
{

	int ret;

	if (!chan || !cookie) {
		wiphy_err(wiphy, "Invalid parameter for ROC\n");
		return -EINVAL;
	}

	ret = obm_remain_on_chan_cfg(wdev->netdev, chan->center_freq,duration);

	if (!ret) {
		*cookie = prandom_u32() | 1;

		cfg80211_ready_on_channel(wdev, *cookie, chan,
					  duration, GFP_ATOMIC);

		wiphy_dbg(wiphy, "info: ROC, cookie = 0x%llx\n", *cookie);
	}

	return ret;
}

/*
 * CFG802.11 operation handler to change interface type.
 */
static int
obm_cfg80211_change_virtual_intf(struct wiphy *wiphy,
				     struct net_device *dev,
				     enum nl80211_iftype type, u32 *flags,
				     struct vif_params *params)
{
	return 0;
}

/*
 * CFG802.11 operation handler to cancel remain on channel.
 */
static int
obm_cfg80211_cancel_remain_on_channel(struct wiphy *wiphy,
					  struct wireless_dev *wdev, u64 cookie)
{
	return 0;
}

#endif
int onebox_set_bitrate_mask(struct wiphy *wiphy,
			    struct net_device *dev,
			    const u8 *peer,
			    const struct cfg80211_bitrate_mask *mask)
{
	int status;

	status = obm_siwrate(wiphy, dev, peer, mask->control->legacy
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0)) &&\
      (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))) 
            ,mask->control->mcs[0]
#elif(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
            ,mask->control->ht_mcs[0]
#endif
            );
	
	return status;
}

static int rsi_to_std_rate_mapping (int rsi_rate)
{

	int rsi_rates[] = {RSI_RATE_1, RSI_RATE_2, RSI_RATE_5_5, RSI_RATE_11, RSI_RATE_6,
										RSI_RATE_9, RSI_RATE_12, RSI_RATE_18, RSI_RATE_24, RSI_RATE_36,
										RSI_RATE_48, RSI_RATE_54};
	int ii = 0;
	for (ii = 0; ii < sizeof(rsi_rates)/sizeof(int); ii++) {
		if (rsi_rate == rsi_rates[ii])
			return ii;
	}
	return -1;
}

static int onebox_dump_station(struct wiphy *wiphy, struct net_device *dev,
			       int idx, u8 *mac, struct station_info *sinfo)
{
	int ret = 0;
	struct local_info data;
	int rate_indx = 0;
	int std_rates_val[] = { RSI_RATE_1M, RSI_RATE_2M, RSI_RATE_5_5M, RSI_RATE_11M, RSI_RATE_6M,
													RSI_RATE_9M, RSI_RATE_12M, RSI_RATE_18M, RSI_RATE_24M, RSI_RATE_36M,
													RSI_RATE_48M, RSI_RATE_54M };

	ret = obm_dump_station_info(dev, &data, mac, sinfo, idx);

	if(ret < 0) {
		return -ENOENT;
	}

	//if (data.mode == NL80211_IFTYPE_AP) {
	if (0) {
		sinfo->filled = BIT(NL80211_STA_INFO_RX_PACKETS);
	} else {
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(4, 0, 0)) 
		sinfo->filled = STATION_INFO_RX_BYTES | 
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 1)) 
				STATION_INFO_STA_FLAGS |
#endif
				STATION_INFO_RX_PACKETS |
				STATION_INFO_RX_BYTES |
				STATION_INFO_TX_PACKETS |
				STATION_INFO_TX_BYTES |
				STATION_INFO_TX_BITRATE | 
				STATION_INFO_RX_BITRATE | 
				STATION_INFO_SIGNAL ;
#else
		sinfo->filled = BIT(NL80211_STA_INFO_RX_BYTES) |
				BIT(NL80211_STA_INFO_RX_PACKETS) |
				BIT(NL80211_STA_INFO_TX_PACKETS) |
				BIT(NL80211_STA_INFO_TX_BYTES)| /*Can be updated later*/
				BIT(NL80211_STA_INFO_TX_BITRATE) |
				BIT(NL80211_STA_INFO_RX_BITRATE) |
				BIT(NL80211_STA_INFO_STA_FLAGS) |
				BIT(NL80211_STA_INFO_SIGNAL);
#endif
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 1)) 
	if (data.wme_ie) {
		sinfo->sta_flags.mask |= BIT(NL80211_STA_FLAG_AUTHORIZED) |
					 BIT(NL80211_STA_FLAG_AUTHENTICATED) |
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)) 
					 BIT(NL80211_STA_FLAG_ASSOCIATED)|
#endif
					 BIT(NL80211_STA_FLAG_WME) ;

		sinfo->sta_flags.set = BIT(NL80211_STA_FLAG_AUTHENTICATED) |
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)) 
				       BIT(NL80211_STA_FLAG_ASSOCIATED) |
#endif
				       BIT(NL80211_STA_FLAG_AUTHORIZED) |
				       BIT(NL80211_STA_FLAG_WME);
	}
#endif
	sinfo->rx_packets = data.rx_data;
	sinfo->rx_bytes = data.rx_bytes;
	sinfo->tx_packets = data.tx_data;
	sinfo->tx_bytes = data.tx_bytes;

	if (data.tx_legacy_rate & 0x100) {
		if (data.tx_legacy_rate & BIT(9)){
			data.tx_legacy_rate &= ~BIT(9);
			sinfo->txrate.flags |= RATE_INFO_FLAGS_SHORT_GI;
		}
		sinfo->txrate.flags |= RATE_INFO_FLAGS_MCS;
		sinfo->txrate.mcs = data.tx_legacy_rate - 0x100;
	} else {
		rate_indx = 0;
		rate_indx = rsi_to_std_rate_mapping(data.tx_legacy_rate);
		if (rate_indx >= 0) {
				sinfo->txrate.legacy =  std_rates_val[rate_indx] * 10;
			}
	}

	if (data.rx_legacy_rate & 0x100) {
		if (data.rx_legacy_rate & BIT(9)){
			data.rx_legacy_rate &= ~BIT(9);
			sinfo->rxrate.flags |= RATE_INFO_FLAGS_SHORT_GI;
		}
		sinfo->rxrate.flags |= RATE_INFO_FLAGS_MCS;
		sinfo->rxrate.mcs = data.rx_legacy_rate - 0x100;
	} else {
		rate_indx = rsi_to_std_rate_mapping(data.rx_legacy_rate);
		if (rate_indx >= 0)
			sinfo->rxrate.legacy = std_rates_val[rate_indx]*10;
	}
	sinfo->signal = data.signal;
	return ret;
}

static struct cfg80211_ops onebox_cfg80211_ops = {
	.set_wiphy_params = onebox_cfg80211_set_wiphy_params,
	.add_virtual_intf = obm_add_virtual_intf,
	.del_virtual_intf = obm_delete_virtual_intf,
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38))
	//.set_tx_power = onebox_cfg80211_set_txpower,
#endif
	.scan = onebox_cfg80211_scan,
	.connect = onebox_cfg80211_connect,
	.disconnect = onebox_cfg80211_disconnect,
	.add_key = onebox_cfg80211_add_key,
	.del_key = onebox_cfg80211_del_key,
	.set_default_key = onebox_cfg80211_set_default_key,
	.get_station = onebox_cfg80211_get_station,
#if(LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
	.set_channel = onebox_cfg80211_set_channel,
#endif
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
	.start_ap = onebox_cfg80211_start_ap,
	.stop_ap = onebox_cfg80211_stop_ap,
	.change_beacon = onebox_cfg80211_change_beacon,
	.mgmt_tx = onebox_mgmt_tx,
#endif
	.del_station = onebox_del_station,
	.deauth = onebox_deauth_station,
	.disassoc = onebox_disassoc_station,
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0))
	.set_mac_acl = onebox_set_mac_acl,
	.start_radar_detection = onebox_start_radar_detection,
#endif
#if 0
	.remain_on_channel = obm_cfg80211_remain_on_channel,
	.cancel_remain_on_channel = obm_cfg80211_cancel_remain_on_channel,
	//.change_virtual_intf = obm_cfg80211_change_virtual_intf,
#endif
	.set_bitrate_mask = onebox_set_bitrate_mask,
	.dump_station = onebox_dump_station,
};

#if(LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38))
static const struct ieee80211_txrx_stypes 
obx_mgmt_stypes[NUM_NL80211_IFTYPES] = {
		[NL80211_IFTYPE_STATION] = {
				.tx = BIT(IEEE80211_STYPE_ACTION >> 4) |
						BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
				.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
						BIT(IEEE80211_STYPE_PROBE_REQ >> 4),
		},
		[NL80211_IFTYPE_AP] = {
				.tx = BIT(IEEE80211_STYPE_ACTION >> 4) |
						BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
				.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
						BIT(IEEE80211_STYPE_PROBE_REQ >> 4),
		},
};  
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0))
int onebox_reg_notifier_apply(struct wiphy *wiphy,
			      struct regulatory_request *request)
#else
void onebox_reg_notifier_apply(struct wiphy *wiphy,
			       struct regulatory_request *request)
#endif
{
	
	if (!request)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0))
		return 0;
#else
		return;
#endif

	/* We always apply this */
	onebox_update_channel_flags(wiphy, wiphy_priv(wiphy));
	onebox_update_country(request->alpha2, wiphy_priv(wiphy));

	switch (request->initiator) {
	case NL80211_REGDOM_SET_BY_CORE:
		/*
		 * If common->reg_world_copy is world roaming it means we *were*
		 * world roaming... so we now have to restore that data.
		 */
		break;
	case NL80211_REGDOM_SET_BY_DRIVER:
		break;
	case NL80211_REGDOM_SET_BY_USER:
		break;
	case NL80211_REGDOM_SET_BY_COUNTRY_IE:
		break;
	}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0))
	return 0;
#else
	return;
#endif
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0))
static int
onebox_regd_init_wiphy(struct wiphy *wiphy,
		       int (*reg_notifier)(struct wiphy *wiphy,
					   struct regulatory_request *request))
#else
static int
onebox_regd_init_wiphy(struct wiphy *wiphy,
		       void (*reg_notifier)(struct wiphy *wiphy,
					    struct regulatory_request *request))
#endif
{
	wiphy->reg_notifier = reg_notifier;
	return 0;
}

/*
 * This function registers the device with CFG802.11 subsystem.
 *
 * The function creates the wireless device/wiphy, populates it with
 * default parameters and handler function pointers, and finally
 * registers the device.
 */
struct cfg80211_priv* cfg80211_wrapper_attach(struct device *dev, u8 *mac_addr, struct ieee80211com *ic, u8 dual_band)
{
	struct cfg80211_priv *cfg_priv;
	struct wiphy *wiphy;
	int ret;

	wiphy = wiphy_new(&onebox_cfg80211_ops, sizeof(struct cfg80211_priv));
	if (!wiphy) {
		dev_err(dev, "%s: creating new wiphy\n", __func__);
		return NULL;
	}

	wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION) |
#if 0
		BIT(NL80211_IFTYPE_P2P_CLIENT) |
		BIT(NL80211_IFTYPE_P2P_GO) |
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
		BIT(NL80211_IFTYPE_P2P_DEVICE) |
#endif
#endif
		BIT(NL80211_IFTYPE_AP);

	memcpy(wiphy->perm_addr, mac_addr, ETH_ALEN);

	wiphy->bands[IEEE80211_BAND_2GHZ] = &obm_band_24ghz;

	if (dual_band)
		wiphy->bands[IEEE80211_BAND_5GHZ] = &obm_band_5ghz;
	else
		wiphy->bands[IEEE80211_BAND_5GHZ] = NULL;

	wiphy->max_scan_ssids = 200;
	wiphy->max_scan_ie_len = 1000;

#if(LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38))
	wiphy->mgmt_stypes = obx_mgmt_stypes;
#endif

#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0))
	wiphy->iface_combinations = &obm_iface_comb_ap_sta;
	wiphy->n_iface_combinations = 1;
	//wiphy->n_addresses = 4;
#else
	wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION) ;
#endif

	/* Initialize cipher suits */
	wiphy->cipher_suites   = cipher_suites;
	wiphy->n_cipher_suites = ARRAY_SIZE(cipher_suites);

	wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;

#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0, 0))
	wiphy->available_antennas_tx = 1;
	wiphy->available_antennas_rx = 1;
#endif

#if 0
	wiphy->features |= NL80211_FEATURE_INACTIVITY_TIMER |
		NL80211_FEATURE_ADVERTISE_CHAN_LIMITS |
		NL80211_FEATURE_FULL_AP_CLIENT_STATE |
		NL80211_FEATURE_LOW_PRIORITY_SCAN;
#endif

#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0))
	wiphy->max_acl_mac_addrs = 32;
#endif

#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
	wiphy->flags |= (WIPHY_FLAG_HAVE_AP_SME | 
			WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD);
#endif

	set_wiphy_dev(wiphy, dev);

	cfg_priv = wiphy_priv(wiphy);
	cfg_priv->wiphy = wiphy;
	cfg_priv->ic = ic;
	onebox_regd_init_wiphy(wiphy, onebox_reg_notifier_apply);

	ret = wiphy_register(wiphy);
	if (ret < 0) {
		dev_err(dev,
				"%s: wiphy_register failed: %d\n", __func__, ret);
		wiphy_free(wiphy);
		return NULL;
	}

	return cfg_priv;
}

#endif


