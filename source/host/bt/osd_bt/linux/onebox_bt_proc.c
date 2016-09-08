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

#include "bt_common.h"
#include "onebox_linux.h"
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/file.h>

ONEBOX_STATIC struct proc_dir_entry *onebox_entry = NULL;
extern uint32 onebox_bt_zone_enabled;

static int onebox_proc_version_show(struct seq_file *seq, void *data)
{
	BT_ADAPTER bt_adapter = seq->private;

	bt_adapter->driver_ver.major = 0;
	bt_adapter->driver_ver.minor = 0;
	bt_adapter->driver_ver.release_num = 9;
	bt_adapter->driver_ver.patch_num = 1;

	seq_printf(seq, 
		   "UMAC : %x.%d.%d.%d\nLMAC : %d.%d.%d.%d.%d\n",
		   bt_adapter->driver_ver.major,
		   bt_adapter->driver_ver.minor, 
		   bt_adapter->driver_ver.release_num, 
		   bt_adapter->driver_ver.patch_num,
		   bt_adapter->lmac_ver.major,
		   bt_adapter->lmac_ver.minor, 
		   bt_adapter->lmac_ver.release_num, 
		   bt_adapter->lmac_ver.patch_num,
		   bt_adapter->lmac_ver.ver.info.fw_ver[0]);
	return 0;
	
}

static int onebox_proc_stats_show(struct seq_file *seq, void *data)
{
	seq_printf(seq, "BULETOOTH STATS\nYet to be developed!\n");
	return 0;
}


static int onebox_proc_debug_zone_show(struct seq_file *seq, void *data)
{
	
	seq_printf(seq,
		   "The zones available are %#x\n",
		   onebox_bt_zone_enabled);
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

		onebox_bt_zone_enabled = dbg_zone;
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

static const struct file_operations proc_debug_zone_ops = {
	.open    = onebox_proc_debug_zone_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.write   = onebox_proc_debug_zone_write,
};	

static const struct file_operations proc_stats_ops = {
	.open    = onebox_proc_stats_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};	

/**
 * This function initializes the proc file system entry.
 *
 * @param bt_adapter   Pointer To Driver Priv Area
 *
 * @return
 * This function return the status success or failure.
 */
ONEBOX_STATUS setup_bt_procfs(BT_ADAPTER bt_adapter)
{
	struct proc_dir_entry *entry = NULL;

	onebox_entry = proc_mkdir("onebox-bt", NULL);
	if (onebox_entry == NULL) {
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
		             ("Unable to create onebox-bt dir entry"));
		return ONEBOX_STATUS_FAILURE;
	} else {
		entry = proc_create_data("version",
					 0,
					 onebox_entry, 
					 &proc_version_ops,
					 bt_adapter);
		if (entry == NULL) {
			ONEBOX_DEBUG(ONEBOX_ZONE_INFO, (TEXT("HAL : Unable to create proc entry [version]\n")));
			return ONEBOX_STATUS_FAILURE;
		}

		entry = proc_create_data("stats",
					 0,
					 onebox_entry,
					 &proc_stats_ops,
					 bt_adapter);
		if (entry == NULL) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("HAL : Unable to create proc entry [stats]\n")));
			return ONEBOX_STATUS_FAILURE;
		}

		entry = proc_create("debug_zone",
				    0,
				    onebox_entry,
				    &proc_debug_zone_ops);
		if (entry == NULL) {
			ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("HAL : Unable to create proc entry [debug_zone]\n")));
			return ONEBOX_STATUS_FAILURE;
		}

	}
	return ONEBOX_STATUS_SUCCESS;
}

/**
 * Removes the previously created proc file entries in the
 * reverse order of creation
 *
 * @param  void
 * @return void 
 */
void destroy_bt_procfs(void)
{
	remove_proc_entry("version", onebox_entry);
	remove_proc_entry("stats", onebox_entry);
	remove_proc_entry("debug_zone", onebox_entry);
	remove_proc_entry("onebox-bt", NULL);
	return;
}
