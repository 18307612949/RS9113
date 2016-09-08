
/**
 * @file onebox_hal_ops.h
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
#include "onebox_linux.h"

struct onebox_osi_bt_ops {
	int32	(*onebox_core_init)(BT_ADAPTER bt_adapter);
	int32	(*onebox_core_deinit)(BT_ADAPTER bt_adapter);
	int32	(*onebox_indicate_pkt_to_core)(BT_ADAPTER bt_adapter,
					       netbuf_ctrl_block_t *netbuf_cb);
	void	(*onebox_dump)(int32 dbg_zone_l, PUCHAR msg_to_print_p,int32 len);
	int32 	(*onebox_bt_xmit) (BT_ADAPTER bt_adapter, 
	       		   netbuf_ctrl_block_t *netbuf_cb);
	int32	(*onebox_snd_cmd_pkt)(BT_ADAPTER  bt_adapter,
	       		      netbuf_ctrl_block_t *netbuf_cb);
	int32	(*onebox_send_pkt)(BT_ADAPTER bt_adapter,
                                   netbuf_ctrl_block_t *netbuf_cb);
	int32	(*onebox_bt_cw)(BT_ADAPTER bt_adapter,
                                   unsigned char *,int );
	int32	(*onebox_bt_ber)(BT_ADAPTER bt_adapter,
                                   unsigned char *,int );
	int32 (*onebox_bt_read_reg_params)(BT_ADAPTER bt_adapter);
};

struct onebox_bt_osd_operations 
{
#ifdef USE_BLUEZ_BT_STACK
  int32	(*onebox_btstack_init)(BT_ADAPTER bt_adapter);
  int32	(*onebox_btstack_deinit)(BT_ADAPTER bt_adapter);
  int32	(*onebox_send_pkt_to_btstack)(BT_ADAPTER bt_adapter, netbuf_ctrl_block_t *netbuf_cb);
#endif
  int32	(*onebox_genl_init)(BT_ADAPTER bt_adapter);
  int32	(*onebox_genl_deinit)(BT_ADAPTER bt_adapter);
  int32	(*onebox_send_pkt_to_genl)(BT_ADAPTER bt_adapter, netbuf_ctrl_block_t *netbuf_cb);
#ifdef RSI_CONFIG_ANDROID
  int32 (*onebox_bdroid_init)(BT_ADAPTER bt_adapter);
  void  (*onebox_bdroid_deinit)(BT_ADAPTER bt_adapter);
#endif
};

/* EOF */
