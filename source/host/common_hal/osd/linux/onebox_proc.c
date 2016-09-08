/**
 * @file onebox_proc.c
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
 * This file contians the proc file system creation & functions for debugging
 * the driver.
 */

#include "onebox_common.h"
#include "onebox_linux.h"
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/file.h>
#include "onebox_zone.h"

/**
 * This function gives the driver and firmware version number.
 *
 * @param  Pointe to the page.
 * @param  start.  
 * @param  off.  
 * @param  Count.  
 * @param  EOF.  
 * @param  data.  
 * @return Returns the number of bytes read. 
 */
static int onebox_proc_version_show(struct seq_file *seq, void *data)
{
	PONEBOX_ADAPTER adapter = seq->private;

	adapter->driver_ver.major = 1;
	adapter->driver_ver.minor = 5;
	adapter->driver_ver.release_num = 0;
	adapter->driver_ver.patch_num = 0;

	seq_printf(seq, 
		   "UMAC : %x.%d.%d.%c\nLMAC : %d.%d.%d.%c\n",
		   adapter->driver_ver.major,
		   adapter->driver_ver.minor, 
		   adapter->driver_ver.release_num, 
		   adapter->driver_ver.patch_num,
		   adapter->lmac_ver.major,
		   adapter->lmac_ver.minor, 
		   adapter->lmac_ver.release_num, 
		   adapter->lmac_ver.patch_num);
		   //adapter->lmac_ver.ver.info.fw_ver[0]);
	return 0;
}	

static void show_assetq_stats(struct seq_file *seq, uint32 asset_id, uint32 idx)
{
	PONEBOX_ADAPTER adapter = seq->private;
	uint8 asset_state[][10] = {
		"ACTIVE",
		"DORMANT",
		"EXTINCT",
	};
	struct driver_assets *d_assets = adapter->d_assets; 

	seq_printf(seq, "    Driver asset state...........: %s\n",
			asset_state[idx]);
	seq_printf(seq, "    Total packets Q'ed...........: %6d\n",
			adapter->tot_pkts_qed[asset_id]);
	seq_printf(seq, "    Remaining packets in Queue...: %6d\n",
			get_skb_queue_len(&adapter->coex_queues[asset_id]));
	seq_printf(seq, "    Total packets sent on air....: %6d\n",
			adapter->tot_pkts_sentout[asset_id]);
	seq_printf(seq, "    Total packets dropped by HAL.: %6d\n\n",
			adapter->tot_pkts_dropped[asset_id]);
	seq_printf(seq, "    TX_ACCESS....................: %6d\n\n",
			d_assets->techs[idx].tx_access);
	
	return;
}

static int onebox_proc_stats_show(struct seq_file *seq, void *data)
{
	uint32 idx = 0;
	PONEBOX_ADAPTER adapter = seq->private;
	struct driver_assets *d_assets = adapter->d_assets; 

#if 0
/* -------> */
	struct ieee80211com *ic = &adapter->vap_com;
	struct ieee80211vap *vap;
	char fsm_state_lp[][32] = { 
		"FSM_CARD_NOT_READY",
		"FSM_FW_LOADED",
		"FSM_LOAD_BOOTUP_PARAMS",
		"FSM_EEPROM_CHECK",
		"FSM_EEPROM_READ_RF_TYPE",
		"FSM_EEPROM_READ_MAC_ADDR",
		"FSM_EEPROM_READ_2P4_PWR_VALS",
		"FSM_EEPROM_READ_5P1_PWR_VALS",
		"FSM_RESET_MAC_CFM",
		"FSM_BB_RF_START",
		"FSM_WAKEUP_SLEEP_VALS",
		"FSM_OPEN",
		"FSM_DEEP_SLEEP_ENABLE",
		"FSM_MAC_INIT_DONE"
	};

	TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) 
	{ 
		if (vap->iv_opmode == IEEE80211_M_STA)
			break;
	}

	seq_printf(seq, "==> ONEBOX DRIVER STATUS <==\n");
	seq_printf(seq, "DRIVER_FSM_STATE: ");
	if (adapter->fsm_state <= FSM_MAC_INIT_DONE)
		seq_printf(seq,
			   "%s",
			   fsm_state_lp[adapter->fsm_state]);
	seq_printf(seq,
		   "(%d)\n\n", 
	           adapter->fsm_state);
#endif

		seq_printf(seq,"   COMMON HAL FSM_STATE.........: %6d\n",
			   d_assets->common_hal_fsm);

		seq_printf(seq,"   COMMON HAL TX_ACCESS.........: %6d\n",
			   d_assets->common_hal_tx_access);
	if (adapter->Driver_Mode == RF_EVAL_MODE_ON)
		seq_printf(seq,
			   "total PER packets sent : %d\n",
			   adapter->total_per_pkt_sent);

	seq_printf(seq, "WLAN QUEUE:\n");

	if (d_assets->techs[WLAN_ID].drv_state == MODULE_ACTIVE)
		idx = 0;
	else if (d_assets->techs[WLAN_ID].drv_state == MODULE_INSERTED)
		idx = 1;
	else
		idx = 2;

	show_assetq_stats(seq, WLAN_Q, idx);
	
	seq_printf(seq, "BLUETOOTH QUEUE:\n");

	if (d_assets->techs[BT_ID].drv_state == MODULE_ACTIVE)
		idx = 0;
	else if (d_assets->techs[BT_ID].drv_state == MODULE_INSERTED)
		idx = 1;
	else
		idx = 2;

	show_assetq_stats(seq, BT_Q, idx);

	seq_printf(seq, "ZIGBEE QUEUE:\n");

	if (d_assets->techs[ZB_ID].drv_state == MODULE_ACTIVE)
		idx = 0;
	else if (d_assets->techs[ZB_ID].drv_state == MODULE_INSERTED)
		idx = 1;
	else
		idx = 2;

	show_assetq_stats(seq, ZIGB_Q, idx);

	seq_printf(seq, "VERY IMPORTANT PACKET(VIP) QUEUE:\n"); // very important packet
	seq_printf(seq, "    Total Packets Q'ed...........: %6d\n",
			adapter->tot_pkts_qed[VIP_Q]);
	seq_printf(seq, "    Remaining packets in queue...: %6d\n",
			get_skb_queue_len(&adapter->coex_queues[VIP_Q]));
	seq_printf(seq, "    Total Packets sent on air....: %6d\n",
			adapter->tot_pkts_sentout[VIP_Q]);
	seq_printf(seq, "    Total Packets dropped by HAL.: %6d\n",
			adapter->tot_pkts_dropped[VIP_Q]);

	return 0;
}


static int onebox_proc_debug_zone_show(struct seq_file *seq, void *data)
{
	
	seq_printf(seq,
		   "The zones available are %#x\n",
		   onebox_zone_enabled);
	return 0;
}

static ssize_t onebox_proc_debug_zone_write(struct file *filp,
					    const char __user *buff,
					    size_t len,
					    loff_t *data)
{
	char user_zone[20] = {0};

	if (!len)
		return 0;

	if (len > 20)
		return -EINVAL;

	if (copy_from_user(user_zone, (void __user *)buff, len)) 
		return -EFAULT;
	else {
		int32 dbg_zone = 0;
		if ((user_zone[0] == '0') &&
		    (user_zone[1] == 'x' || 
		     user_zone[1] == 'X'))
			dbg_zone = simple_strtol(&user_zone[2], NULL, 16);
		else
			dbg_zone = simple_strtol(user_zone, NULL, 10);

		onebox_zone_enabled = dbg_zone;
	}
	return len;
}


static int onebox_proc_version_open(struct inode *inode, struct file *file)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
	return single_open(file,
			   onebox_proc_version_show,
			   PDE_DATA(inode));
#else
	return single_open(file,
			   onebox_proc_version_show,
			   PDE(inode)->data);
#endif
}

static int onebox_proc_stats_open(struct inode *inode, struct file *file)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
	return single_open(file,
			   onebox_proc_stats_show,
			   PDE_DATA(inode));
#else
	return single_open(file,
			   onebox_proc_stats_show,
			   PDE(inode)->data);
#endif
}

static int onebox_proc_debug_zone_open(struct inode *inode, struct file *file)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
	return single_open(file,
			   onebox_proc_debug_zone_show,
			   PDE_DATA(inode));
#else
	return single_open(file,
			   onebox_proc_debug_zone_show,
			   PDE(inode)->data);
#endif
}

static const struct file_operations proc_version_ops = {
	.open    = onebox_proc_version_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};	

static const struct file_operations proc_stats_ops = {
	.open    = onebox_proc_stats_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};	

static const struct file_operations proc_debug_zone_ops = {
	.open    = onebox_proc_debug_zone_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.write   = onebox_proc_debug_zone_write,
};	




static int onebox_proc_sdio_stats_show(struct seq_file *seq, void *data)
{

	PONEBOX_ADAPTER adapter = seq->private;

	seq_printf(seq,
		   "total_sdio_interrupts: %d\n",
		   adapter->sdio_int_counter);
	seq_printf(seq,
		   "total_sdio_interrupts_zero_status: %d\n",
		   adapter->sdio_intr_status_zero);
	seq_printf(seq,
		   "sdio_msdu_pending_intr_count: %d\n", 
		   adapter->total_sdio_msdu_pending_intr);
	seq_printf(seq,
		   "sdio_buff_status_count : %d\n",
		   adapter->buf_status_interrupts);
	seq_printf(seq,
		   "sdio_unknown_intr_count: %d\n",
		   adapter->total_sdio_unknown_intr);
	
	return 0;
}
static int onebox_proc_sdio_stats_open(struct inode *inode, struct file *file)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
	return single_open(file,
			   onebox_proc_sdio_stats_show,
			   PDE_DATA(inode));
#else
	return single_open(file,
			   onebox_proc_sdio_stats_show,
			   PDE(inode)->data);
#endif
}

static const struct file_operations proc_sdio_stats_ops = {
	.open    = onebox_proc_sdio_stats_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};	

/**
 * This function initializes the proc file system entry.
 *
 * @param adapter   Pointer To Driver Priv Area
 *
 * @return
 * This function return the status success or failure.
 */
struct proc_dir_entry* init_proc_fs(void *adapter, uint8 *name)
{
	struct proc_dir_entry *entry = NULL;
	struct proc_dir_entry *onebox_entry;

	onebox_entry = proc_mkdir(name, NULL);
	if (onebox_entry == NULL) {
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
		             ("onebox_init_proc: Unable to create dir entry"));
			return NULL;
	} else {
		entry = proc_create_data("version",
					 0,
					 onebox_entry, 
					 &proc_version_ops,
					 adapter);
		if (entry == NULL) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("HAL : Unable to create proc entry [version]\n"));
			return NULL;
		}

		entry = proc_create_data("stats",
					 0,
					 onebox_entry,
					 &proc_stats_ops,
					 adapter);
		if (entry == NULL) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("HAL : Unable to create proc entry [stats]\n"));
			return NULL;
		}

		entry = proc_create("debug_zone",
				    0,
				    onebox_entry,
				    &proc_debug_zone_ops);
		if (entry == NULL) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("HAL : Unable to create proc entry [debug_zone]\n"));
			return NULL;
		}

		entry = proc_create_data("sdio_stats",
				0,
				onebox_entry,
				&proc_sdio_stats_ops, 
				adapter);
		if (entry == NULL) {
			ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("HAL : Unable to create proc entry [debug_zone]\n"));
			return NULL;
		}

	}
	return onebox_entry;
}

/**
 * Removes the previously created proc file entries in the
 * reverse order of creation
 *
 * @param  void
 * @return void 
 */
void proc_entry_remove(struct proc_dir_entry *onebox_entry, uint8 *proc_name)
{
	ONEBOX_DEBUG( ONEBOX_ZONE_INFO, ("HAL :Removing HAL procfs\n"));
	remove_proc_entry("version", onebox_entry);
	remove_proc_entry("stats", onebox_entry);
	remove_proc_entry("debug_zone", onebox_entry);
	remove_proc_entry("sdio_stats", onebox_entry);
	remove_proc_entry(proc_name, NULL);

	return;
}
