#include "onebox_wlan_core.h"

/* ***************** Core 2 HAL functions **************** */
/**
 * This function updates the autorate stats.
 *
 * @param Pointer to the driver private structure.
 * @param Pointer to transmit stats structure.
 * @return ONEBOX_STATUS_SUCCESS on success else negative number on failure.
 */
uint32 core_update_tx_status(WLAN_ADAPTER w_adapter,struct tx_stat_s *tx_stat)
{
	int i;

	for(i=0;i<tx_stat->count;i++) 
	{
		w_adapter->autorate_stats[tx_stat->staid][tx_stat->stat[i].rate_idx].total_success = tx_stat->stat[i].success;
		w_adapter->autorate_stats[tx_stat->staid][tx_stat->stat[i].rate_idx].total_attempts = tx_stat->stat[i].attempts;
	}
	return ONEBOX_STATUS_SUCCESS;
}

/* Tx data done processing */
/**
 * This function process the data packets to send.
 *
 * @param Pointer to the Adapter structure.
 * @param Pointer to netbuf control block structure.
 * @return ONEBOX_STATUS_SUCCESS on success else negative number on failure.
 */
uint32 core_tx_data_done(WLAN_ADAPTER w_adapter,netbuf_ctrl_block_t *netbuf_cb)
{
	if(netbuf_cb) 
	{
		w_adapter->os_intf_ops->onebox_free_pkt(netbuf_cb,0);
	} 
	else 
	{
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,(TEXT("\nCORE_MSG: tx done error")));
	}
	return ONEBOX_STATUS_SUCCESS;
}

/**
 * This function does the core initialization.
 *
 * @param pointer to the Adapter structure.
 * return ONEBOX_STATUS_SUCCESS on success else negative number on failure. 
 *
 */
uint32 core_init(WLAN_ADAPTER w_adapter)
{
	uint32 count;
	//w_adapter->init_done = 0;

	for (count = 0; count < ONEBOX_VAPS_DEFAULT; count++) 
	{
		w_adapter->sec_mode[count] = IEEE80211_CIPHER_NONE;
	}

	/* attach net80211 */
	//core_net80211_attach(w_adapter);

	/* Rate adaptation initialization */
	ONEBOX_DEBUG(ONEBOX_ZONE_INFO,(TEXT("%s:Registering Autorate\n"),__func__));
	//w_adapter->sc_rc = (struct onebox_ratectrl *)core_rate_attach(w_adapter);
	if (w_adapter->sc_rc == NULL) 
	{
		//goto out;
	}
	w_adapter->core_init_done = 1;
	//w_adapter->init_done = 1;
	return ONEBOX_STATUS_SUCCESS;
}

/**
 * This functions deinitializes the core.
 *
 * @param Pointer to driver private structure.
 * return ONEBOX_STATUS_SUCCESS on success else negative number on failure. 
 */
uint32 core_deinit(WLAN_ADAPTER w_adapter)
{ 
	uint8 ii;

	//struct ieee80211com *ic = &w_adapter->vap_com;
	w_adapter->core_init_done = 0;

	/* Detach net80211 */
	/* Clean up the Net80211 Module */
	core_net80211_detach(w_adapter);
	/* Purge the mgmt transmit queue */

	/* Purge the Data transmit queue */
	for (ii = 0; ii < NUM_SOFT_QUEUES; ii++)
	{
		w_adapter->os_intf_ops->onebox_queue_purge(&w_adapter->host_tx_queue[ii]);
	}

	/* detach autorate */
	//core_rate_detach(w_adapter);

	return ONEBOX_STATUS_SUCCESS;
}
