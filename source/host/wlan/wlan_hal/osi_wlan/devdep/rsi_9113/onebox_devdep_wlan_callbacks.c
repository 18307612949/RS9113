
#include "wlan_common.h"
#include "onebox_linux.h"
#include "onebox_wlan_pktpro.h"

static struct onebox_devdep_operations devdep_ops =
{
	/* In case if u want to change the order of these member assignments, you should change
	 * the order in the declarations also */
	NULL,//interrupt_handler,          /* .onebox_handle_interrupt          = interrupt_handler,*/
	NULL,//load_ta_instructions,       /* .onebox_load_ta_instructions      = load_ta_instructions,*/
	onebox_mgmt_pkt_recv,       /* .onebox_snd_cmd_pkt               = onebox_mgmt_pkt_recv,*/
	//send_onair_mgmt_pkt,        /* .onebox_snd_mgmt_pkt              = send_onair_mgmt_pkt,*/
	send_beacon,                /* .onebox_send_beacon               = send_beacon,*/
	send_onair_data_pkt,        /* .onebox_send_data_pkt             = send_onair_data_pkt,*/
	NULL,//send_bt_pkt, /*.onebox_send_pkt = send_bt_pkt */
	NULL,//device_init,                /* .onebox_device_init               = device_init,*/
	NULL,//device_deinit,              /* .onebox_device_deinit             = device_deinit,*/
	set_channel,                /* .onebox_set_channel               = set_channel,*/
	bb_reset_req, 
	hal_set_sec_wpa_key,        /* .onebox_hal_set_sec_wpa_key       = hal_set_sec_wpa_key,*/
	hal_send_sta_notify_frame,  /* .onebox_hal_send_sta_notify_frame = hal_send_sta_notify_frame,*/
	schedule_pkt_for_tx,        /* .onebox_schedule_pkt_for_tx       = schedule_pkt_for_tx,*/
	sdio_scheduler_thread,      /* .onebox_sdio_scheduler_thread     = sdio_scheduler_thread,*/
//	send_qos_conf_frame,        /*  onebox_send_qos_conf_frame       = send_qos_conf_frame,*/
	program_bb_rf,              /*  onebox_program_bb_rf             = program_bb_rf*/
	update_device_op_params,    /*  onebox_update_device_op_params   = update_device_op_params */
//	start_tx_rx,                /*  onebox_start_tx_rx               = start_tx_rx */
	start_autorate_stats,       /*  onebox_start_autorate_stats      = start_autorate_stats */
	hal_load_key,               /*  onebox_hal_load_key              = hal_load_key*/
	set_vap_capabilities,       /*  onebox_set_vap_capabilities      = set_vap_capabilities */
	//hw_queue_status,          /*  onebox_hw_queue_status           = hw_queue_status*/
	band_check,         	    /*  onebox_band_check                = band_check*/
	set_bb_rf_values,         	/*  onebox_set_bb_rf_values          = set_bb_rf_values*/
#ifdef PWR_SAVE_SUPPORT
//	onebox_send_ps_params,		/*  onebox_send_ps_params		     = onebox_send_ps_params */
	//reissue_ioctl,
	//update_traffic_psp_params,
	//update_uapsd_params,
#endif
	set_cw_mode,         	    /*  onebox_cw_mode          = set_bb_rf_values*/
	onebox_send_internal_mgmt_frame,  /* onebox_send_internal_mgmt_frame = send_internal_mgmt_frame */
	eeprom_read,         	      /*  onebox_eeprom_rd                 = eeprom_read */
	NULL,//flash_write,  			/*onebox_flash_write              = flash_write */
#ifdef BYPASS_TX_DATA_PATH
	onebox_send_block_unblock,
#endif
	onebox_ant_sel,         	    /*  onebox_program_ant_sel          = onebox_ant_sel*/
	onebox_do_master_ops,					/*	onebox_do_master_ops						=	onebox_do_master_ops*/
	onebox_send_debug_frame,
	onebox_conf_beacon_recv,
};

struct onebox_devdep_operations *onebox_get_devdep_wlan_operations(void)
{
	return (&devdep_ops);
}
EXPORT_SYMBOL(onebox_get_devdep_wlan_operations);
