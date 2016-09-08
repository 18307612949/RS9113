/**
 * @file    rsi_driver_event_handlers.c
 * @version 0.1
 * @date    15 Aug, 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contain the event handlers
 *
 *  @section Description  This file contains event handlers to handle transfer application 
 *  and receive event from module. 
 *
 *
 */
/**
 * Include files
 * */
#include "rsi_driver.h"
#include "rsi_wlan_config.h"
#include "platform_specific.h"
#include "rsi_nl_app.h"
#ifdef RSI_UART_INTERFACE

#endif

//! Sleep Ack frame
const uint8_t rsi_sleep_ack[RSI_FRAME_DESC_LEN]      =    {0x00, 0x40, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*==============================================*/
/**
 * @fn          void rsi_tx_event_handler(void)
 * @brief       Handles TX event
 * @param[in]   None, 
 * @param[out]  None,
 * @return
 *
 *
 * @section description
 * This funciton retrieves the packet from protocol TX pending queue
 * and forwards to the module.
 *
 *
 */
/*void rsi_tx_event_handler(void)
{
  rsi_pkt_t  *pkt = NULL;
  uint8_t    *buf_ptr;
  uint16_t   length = 0;
#ifdef RSI_SPI_INTERFACE
  uint16_t   retval = 0;
  uint8_t    int_status = 0;
#endif

  //!Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;  

  uint8_t    common_pkt_pending = 0;
  uint8_t    bt_pkt_pending = 0;
  uint8_t    wlan_pkt_pending = 0;
  uint8_t    zb_pkt_pending = 0;
  uint8_t    queueno = 0xff;

  /* First Check for Common Queue */
 /* common_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->common_tx_q);

#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE)
  //! check for packet pending in bt/ble common queue 
  if(rsi_driver_cb->bt_single_tx_q != NULL)
  {
    bt_pkt_pending = rsi_check_queue_status(rsi_driver_cb->bt_single_tx_q);
  }

#endif

#ifdef RSI_ZB_ENABLE
  //! check for packet pending in zb queue
  zb_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->zigb_tx_q);
#endif

  //! check for packet pending in wlan queue
  wlan_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->wlan_tx_q);

  if(rsi_common_cb->power_save.power_save_enable && (common_pkt_pending || wlan_pkt_pending || bt_pkt_pending || zb_pkt_pending))
  	{
#if (RSI_HAND_SHAKE_TYPE == GPIO_BASED)
  //! request wakeup if module is in GPIO_BASED handshake power save
#if (RSI_WMM_PS_ENABLE && RSI_WMM_PS_TYPE)
  rsi_wait4wakeup();
#else
  rsi_req_wakeup();
#endif
#else
#if (RSI_ULP_MODE == 1)
      rsi_ulp_wakeup_init();
#endif
#endif
    }
    
  if(common_pkt_pending)
  {
    //! if packet pending dequeue the packet from common queue
    pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->common_tx_q);
  } 
#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE)
  else if(bt_pkt_pending)
  {
    //! if packet pending dequeue the packet from bt/ble common queue
    pkt = (rsi_pkt_t *)rsi_dequeue_pkt(rsi_driver_cb->bt_single_tx_q);  
  }
#endif
#ifdef RSI_ZB_ENABLE
  else if(zb_pkt_pending)
  {
    //! if packet pending dequeue the packet from zb queue
    pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->zigb_tx_q);
  }
#endif  
  else if(wlan_pkt_pending)
  {
#ifdef RSI_SPI_INTERFACE
    //! Read interrupt status register to check buffer full condition
    retval = rsi_device_interrupt_status(&int_status);

    if(retval != 0x0) 
    {
      //! if SPI fail the return
      return;
    }

    //! Check buffer full
    if(int_status & RSI_BUFFER_FULL)
    {
      //! if buffer full then return without clearing Tx event
      return;
    }
#endif
    //! dequeue the packet from wlan queue
    pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_driver_cb->wlan_tx_q);
  }

  //! Check any packet pending in protocol queue
  if(common_pkt_pending || wlan_pkt_pending || bt_pkt_pending || zb_pkt_pending) 
  {
    buf_ptr = (uint8_t *)pkt->desc;

    //! Get protocol type
    queueno =  (buf_ptr[1] & 0xf0) >> 4;

    //! Get packet length
    length = (*(uint16_t *)buf_ptr & 0x0FFF);

    if (buf_ptr[2] == RSI_COMMON_REQ_PWRMODE)
    {
      //! Mask Tx events while sending pwr mode req
      rsi_mask_event(RSI_TX_EVENT);

      //! Adding small delay
      rsi_delay_ms(3);
    }

    // Writing to Module
    rsi_frame_write((rsi_frame_desc_t *)buf_ptr, &buf_ptr[RSI_HOST_DESC_LENGTH], length);
    printf("\n ### Written on UART ###");
    if(wlan_pkt_pending)
    {
      //! signal semaphore incase of packet having async response
      rsi_wlan_packet_transfer_done(pkt);
    }
    else if(bt_pkt_pending)
    {
#if (defined RSI_BT_ENABLE ||defined RSI_BLE_ENABLE)
      if (queueno == RSI_BT_Q)
      {     
        rsi_bt_common_tx_done(pkt);
      }
#endif      
    }
	else if(zb_pkt_pending)
	{
    printf("\n ### zb tx done 1, queue:%d ###",queueno);
#ifdef RSI_ZB_ENABLE	
      if (queueno == RSI_ZB_Q)
      {     
    printf("\n ### zb tx done 2 ###");
        rsi_zb_tx_done(pkt);
      }

#endif
	}
  } 
  else
  {
	  if(rsi_common_cb->power_save.power_save_enable)
	  {
#if (RSI_HAND_SHAKE_TYPE == GPIO_BASED)

		  //! Keep Sleep confirm GPIO low
		  rsi_allow_sleep();

#elif (RSI_HAND_SHAKE_TYPE == MSG_BASED)

		  if (rsi_common_cb->power_save.module_state == RSI_SLP_RECEIVED)
		  {
			  //! Send ACK if POWERMODE 3 and 9,incase of powermode 2 and 8 make GPIO low
			  if (rsi_frame_write(rsi_sleep_ack,NULL,NULL))
			  {
				  //! Handle failure
			  }
			  rsi_mask_event(RSI_TX_EVENT);
			  rsi_common_cb->power_save.module_state = RSI_SLP_ACK_SENT;
		  }
#endif
	  }
    //! signal semaphore incase of packet having async response
        rsi_common_packet_transfer_done(pkt);
	  
    rsi_clear_event(RSI_TX_EVENT);
  }
}*/


/*==============================================*/
/**
 * @fn         void rsi_rx_event_handler(void)
 * @brief      Handles RX event
 * @param[in]  None 
 * @param[out] None
 * @return     None
 *
 *
 * @section description
 * This function reads packet from module and process RX packet 
 *
 *
 */

void rsi_rx_event_handler(void)
{
  uint8_t   queue_no;
  uint8_t   frame_type;
  uint16_t  status = 0;
  uint8_t   *buf_ptr = NULL;
  rsi_pkt_t *rx_pkt = NULL;

  //!Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  //! Allocate packet to receive packet from module
  rx_pkt = rsi_pkt_alloc(&rsi_driver_cb->rx_pool);
  if(rx_pkt == NULL)
  {
    //! rx_pkt not available mask rx event
    rsi_mask_event(RSI_RX_EVENT);
    return;
  }

  buf_ptr = (uint8_t *)rx_pkt->desc;

    if(rsi_common_cb->power_save.power_save_enable)
    {
#ifdef RSI_SPI_INTERFACE
#if (RSI_SELECT_LP_OR_ULP_MODE != RSI_LP_MODE)
      rsi_ulp_wakeup_init();
#endif
#endif
    }

  //! Read packet from module
   status = rsi_frame_read(buf_ptr);

   //! Allow Power save again
if(rsi_common_cb->power_save.power_save_enable)
    {
#if (RSI_HAND_SHAKE_TYPE == GPIO_BASED)
      rsi_allow_sleep();
#endif
    }

  if(status)
  {
#ifdef RSI_UART_INTERFACE
  if(!rsi_check_queue_status(&rsi_linux_app_cb.rcv_queue))
#endif
    rsi_clear_event(RSI_RX_EVENT);
    rsi_pkt_free(&rsi_driver_cb->rx_pool,rx_pkt);
    rsi_hal_intr_unmask();
    return;
  }

  //! Extract the queue number from the receivec frame
  queue_no = ((buf_ptr[1] & 0xF0) >> 4);

  //! Extract the frame type from the received frame
  frame_type = *(uint8_t*)(buf_ptr + RSI_RESP_OFFSET);
#if (RSI_HAND_SHAKE_TYPE == MSG_BASED)
  if ((frame_type == RSI_RSP_SLP) || (frame_type == RSI_RSP_WKP))
  {
    rsi_handle_slp_wkp(frame_type); 
    rsi_pkt_free(&rsi_driver_cb->rx_pool,rx_pkt);
#if RSI_UART_INTERFACE
  if(!rsi_check_queue_status(&rsi_linux_app_cb.rcv_queue))
#endif
    rsi_clear_event(RSI_RX_EVENT);
    //! Unmask RX event
    rsi_unmask_event(RSI_RX_EVENT);

    rsi_hal_intr_unmask();
    return;
  }
#endif

  if((queue_no == RSI_WLAN_MGMT_Q) && ((frame_type == RSI_COMMON_RSP_CARDREADY)
        || (frame_type == RSI_COMMON_RSP_OPERMODE) || (frame_type == RSI_COMMON_RSP_PWRMODE)
        || (frame_type == RSI_COMMON_RSP_ANTENNA_SELECT) || (frame_type == RSI_COMMON_RSP_SOFT_RESET)))
  {
    //! Process common packet
    rsi_driver_process_common_recv_cmd(rx_pkt);
  }
  else if((queue_no == RSI_BT_Q) || (queue_no == RSI_ZB_Q))
  {
    //! Process BT/ZB packet
#ifdef RSI_ZB_ENABLE
    rsi_driver_process_zigb_recv_cmd(rx_pkt);
    rsi_free(g_rcvnlink);
#endif
#if (defined RSI_BT_ENABLE ||defined RSI_BLE_ENABLE)
    rsi_driver_process_bt_resp_handler(rx_pkt);
#endif	
  }
  else if(queue_no == RSI_WLAN_MGMT_Q) 
  {
    //! Process WLAN packet
#ifndef RSI_ZB_ENABLE
    rsi_driver_process_wlan_recv_cmd(rx_pkt);
#endif
  } 
  else if(queue_no == RSI_WLAN_DATA_Q)
  {
#if RSI_TCP_IP_BYPASS
      //! Process Raw DATA packet
      rsi_wlan_process_raw_data(rx_pkt);
#else
      //! Process DATA packet
#ifndef RSI_ZB_ENABLE
      rsi_driver_process_recv_data(rx_pkt);
#endif
#endif
  }

  //! Free the packet after processing
  rsi_pkt_free(&rsi_driver_cb->rx_pool,rx_pkt);

#ifdef RSI_UART_INTERFACE
  if(!rsi_check_queue_status(&rsi_linux_app_cb.rcv_queue))
#endif
  //! Clear RX event
  rsi_clear_event(RSI_RX_EVENT);

  //! Unmask RX event
  rsi_unmask_event(RSI_RX_EVENT);

  rsi_hal_intr_unmask();
  return;
}

int16_t rsi_frame_read(uint8_t *pkt_buffer)
{

  rsi_pkt_t *rx_pkt = NULL;

  uint16_t length;

  //! dequeue the packet from application control block
  rx_pkt = (rsi_pkt_t *)rsi_dequeue_pkt(&rsi_linux_app_cb.rcv_queue);

  if(rx_pkt)
  {
    //! calculate the length from the packet
    length = (rx_pkt->desc[0] | ((rx_pkt->desc[1] & 0xF)<<8)) + RSI_FRAME_DESC_LEN;

    //! copy the data 
    memcpy(pkt_buffer,&(rx_pkt->desc),length);

    //! Free the packet allocated
    free(rx_pkt);


    return 0;
  }

  return -1;

}


