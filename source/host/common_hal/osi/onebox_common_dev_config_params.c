/**
 * @file   onebox_common_pwr_sve_cfg_params.c
 * @author Jahnavi Meher
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
 * The file contains the user configurable values that are required by the power
 * save module in the firmware.
 */

/* include files */
#include "onebox_common.h"
#include "onebox_linux.h"
#include "onebox_mgmt.h"
#include "onebox_intf_ops.h"
#include "onebox_sdio_intf.h"
#include "onebox_zone.h"

/* The following structure is used to configure values */
struct rsi_config_vals_s dev_config_vals[] = {
	 {.lp_sleep_handshake = 0,
	  .ulp_sleep_handshake = 0,
	  .sleep_config_params = 0,
	  .host_wakeup_intr_enable = 0, 
	  .host_wakeup_intr_active_high = 0,
	  .ext_pa_or_bt_coex_en = 0,
	  //.lp_wakeup_threshold = 0,
	  //.ulp_wakeup_threshold = 5000 
	  },
};

rsi_ulp_gpio_vals unused_ulp_gpio_bitmap = {

    .Motion_sensor_GPIO_ULP_Wakeup = UNUSED_GPIO,
    .Sleep_indication_from_device = UNUSED_GPIO,
    .ULP_GPIO_2 = UNUSED_GPIO,
    .Push_Button_ULP_Wakeup = UNUSED_GPIO,
};

rsi_soc_gpio unused_soc_gpio_bitmap = {
    
        .PSPI_CSN_0                                  = USED_GPIO, //GPIO_0	
        .PSPI_CSN_1                                  = USED_GPIO, //GPIO_1	
        .host_wakeup_intr                            = UNUSED_GPIO, //GPIO_2	 Enable this if host_intr_wakeup is enabled.
        .PSPI_DATA_0                                 = USED_GPIO, //GPIO_3	
        .PSPI_DATA_1                                 = USED_GPIO, //GPIO_4	
        .PSPI_DATA_2                                 = USED_GPIO, //GPIO_5	
        .PSPI_DATA_3                                 = USED_GPIO, //GPIO_6	
        .I2C_SCL                                     = USED_GPIO, //GPIO_7	
        .I2C_SDA                                     = USED_GPIO, //GPIO_8	          	
        .UART1_RX                                    = UNUSED_GPIO, //GPIO_9	
        .UART1_TX                                    = UNUSED_GPIO, //GPIO_10	
        .UART1_RTS_I2S_CLK                           = UNUSED_GPIO, //GPIO_11	
        .UART1_CTS_I2S_WS                            = UNUSED_GPIO, //GPIO_12	
        .Debug_UART_RX_I2S_DIN                       = UNUSED_GPIO, //GPIO_13	 Disable able this if uart debug is required.
        .Debug_UART_TX_I2S_DOUT                      = UNUSED_GPIO, //GPIO_14	 Disable this if uart debug is required.
        .LP_Wakeup_Boot_Bypass                       = UNUSED_GPIO, //GPIO_15	
        .LED_0                                       = USED_GPIO, //GPIO_16	Disable this if Led glow is required during TX/RX. 
        .BT_Coexistance_WLAN_ACTIVE_EXT_PA_ANT_SEL_A = UNUSED_GPIO, //GPIO_17	Disable this for BT_WIFI COEX
        .BT_Coexistance_BT_PRIORITY_EXT_PA_ANT_SEL_B = UNUSED_GPIO, //GPIO_18	Disable this for BT_WIFI COEX
        .BT_Coexistance_BT_ACTIVE_EXT_PA_ON_OFF      = UNUSED_GPIO, //GPIO_19	Disable this for EXT_PA 
        .RF_reset                                    = USED_GPIO, //GPIO_20	This should be always used.
        .Sleep_indication_from_device                = UNUSED_GPIO, //GPIO_21	For GPIO handhsake this should be used
};

/**
 * configure_common_dev_params() - This function is used to send a frame to the
 * 			           firmware to configure some device parameters.
 * 			           The params are picked from the struct above.
 *
 * @adapter: The common driver structure.
 *
 * Return: ONEBOX_STATUS_SUCCESS on successful writing of frame, else 
 * 	   ONEBOX_STATUS_FAILURE
 */

ONEBOX_STATUS onebox_configure_common_dev_params(PONEBOX_ADAPTER adapter)
{

	onebox_mac_frame_t *mgmt_frame;
	netbuf_ctrl_block_t *netbuf_cb = NULL;
	ONEBOX_STATUS status = ONEBOX_STATUS_SUCCESS;
	uint16 *frame_body;
	uint32 *unused_soc_gpio ;
	uint16 *unused_ulp_gpio ;
	struct driver_assets *d_assets = adapter->d_assets;
	uint8 extnd_size = 0;

	netbuf_cb = adapter->os_intf_ops->onebox_alloc_skb(FRAME_DESC_SZ + sizeof(rsi_config_vals));
	if(netbuf_cb == NULL)
	{            
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, (TEXT("%s: Unable to allocate skb\n"), __func__));
		return ONEBOX_STATUS_FAILURE;

	}    
	adapter->os_intf_ops->onebox_add_data_to_skb(netbuf_cb, (FRAME_DESC_SZ + sizeof(rsi_config_vals)));
	extnd_size = ((unsigned long )netbuf_cb->data & 0x3f);  /* To make it 64bit aligned buffer, to avoid mmc timeout interrupt */
	adapter->os_intf_ops->onebox_change_hdr_size(netbuf_cb, extnd_size);
	adapter->os_intf_ops->onebox_memset(netbuf_cb->data,0,(FRAME_DESC_SZ + extnd_size));

	mgmt_frame = (onebox_mac_frame_t *)&netbuf_cb->data[0];
	frame_body = (uint16 *)&netbuf_cb->data[16];
	adapter->os_intf_ops->onebox_memset(mgmt_frame, 0, (FRAME_DESC_SZ + sizeof(rsi_config_vals)));

	mgmt_frame->desc_word[0] = ONEBOX_CPU_TO_LE16((sizeof(rsi_config_vals) + extnd_size) 
	                                              | (COEX_TX_Q << 12));
	netbuf_cb->tx_pkt_type = COEX_Q;
	mgmt_frame->desc_word[1] = ONEBOX_CPU_TO_LE16(COMMON_DEV_CONFIG); 
	if (d_assets->lp_ps_handshake_mode == PACKET_HAND_SHAKE) {
		ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("Ivalid Configuration of PACKET_HAND_SHAKE in LP_MODE\n"));
		return ONEBOX_STATUS_FAILURE;
	}

	frame_body[0] = (uint16 )d_assets->lp_ps_handshake_mode;
	frame_body[0] |= (uint16 )d_assets->ulp_ps_handshake_mode << 8;

	if (d_assets->rf_power_val == RF_POWER_3_3) {
	    frame_body[1] = BIT(1);
  }

  if(dev_config_vals[0].host_wakeup_intr_enable) { 
      unused_soc_gpio_bitmap.host_wakeup_intr = USED_GPIO;
      //frame_body[1] |= (((uint16 )(dev_config_vals[0].host_wakeup_intr_enable)) << 8);
      frame_body[1] |= BIT(2); //HOST_WAKEUP_INTR_EN
      if(dev_config_vals[0].host_wakeup_intr_active_high)
      {
          frame_body[1] |= BIT(3); //HOST_WAKEUP_INTR_ACTIVE_HIGH

      }
  }

  unused_ulp_gpio = (uint16 *)&unused_ulp_gpio_bitmap;
  //frame_body[1] |= ((uint16)unused_ulp_gpio_bitmap << 8 );
  frame_body[1] |= ((*unused_ulp_gpio) << 8);

  if ((d_assets->lp_ps_handshake_mode == GPIO_HAND_SHAKE) ||
          (d_assets->ulp_ps_handshake_mode == GPIO_HAND_SHAKE)) {
       
      if ((d_assets->lp_ps_handshake_mode == GPIO_HAND_SHAKE)) {
          unused_soc_gpio_bitmap.LP_Wakeup_Boot_Bypass = USED_GPIO;
      }

      if ((d_assets->ulp_ps_handshake_mode == GPIO_HAND_SHAKE)) {
          unused_ulp_gpio_bitmap.Motion_sensor_GPIO_ULP_Wakeup = USED_GPIO;
      }


      if ((d_assets->device_gpio_type == TA_GPIO)) {
          ONEBOX_DEBUG(ONEBOX_ZONE_ERROR, 
                  (TEXT("%s : Line %d Invalid Configuration OF ULP_PS_HANDHSAKE_MODE Enabling Sleep_Indication_from_device GPIO \n"),
                   __func__, __LINE__));
          unused_soc_gpio_bitmap.Sleep_indication_from_device = USED_GPIO;
      } else {
          unused_ulp_gpio_bitmap.Sleep_indication_from_device = USED_GPIO;
          frame_body[1] |= BIT(0); //ULP_GPIO for Handshake
      }
  }          


  dev_config_vals[0].opermode = d_assets->oper_mode;

  if(dev_config_vals[0].ext_pa_or_bt_coex_en ) {
       frame_body[4] = (uint8 )dev_config_vals[0].ext_pa_or_bt_coex_en;

       unused_soc_gpio_bitmap.BT_Coexistance_WLAN_ACTIVE_EXT_PA_ANT_SEL_A = USED_GPIO;
       unused_soc_gpio_bitmap.BT_Coexistance_BT_PRIORITY_EXT_PA_ANT_SEL_B = USED_GPIO;
       unused_soc_gpio_bitmap.BT_Coexistance_BT_ACTIVE_EXT_PA_ON_OFF = USED_GPIO;
  }
       frame_body[4] |= ((uint16 )dev_config_vals[0].opermode << 8);
       frame_body[5] |= ((uint16 )d_assets->wlan_rf_power_mode);
       frame_body[5] |= ((uint16 )d_assets->bt_rf_power_mode << 8);
       frame_body[6] |= ((uint16 )d_assets->zigb_rf_power_mode);
       frame_body[6] |= ((uint16 )adapter->Driver_Mode << 8);
       frame_body[7] = ((uint16 )d_assets->region_code);
       frame_body[7] |= ((uint16 )d_assets->obm_ant_sel_val << 8);

  unused_soc_gpio = (uint32 *)&unused_soc_gpio_bitmap;
  *(uint32 *)&frame_body[2] = *unused_soc_gpio;

	adapter->coex_osi_ops->onebox_dump(ONEBOX_ZONE_DEBUG,
					   (uint8 *)mgmt_frame,
					   FRAME_DESC_SZ + sizeof(rsi_config_vals)) ;
	send_pkt_to_coex(d_assets, netbuf_cb, COEX_Q);
	return status;


}
EXPORT_SYMBOL(onebox_configure_common_dev_params);
