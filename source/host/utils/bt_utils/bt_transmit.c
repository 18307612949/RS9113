/**
 * @file transmit.c 
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
 * This file handles the transmission of Per frames to the PPE in continuous/Burst mode
 * as per user's choice
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include "bt_util.h"

#define NO_OF_ARGS  17
#define STOP_OF_ARGS 2




int main(int argc, char *argv[])
{      
	int i, rc = 0;
	char *tmp_rate;
	bt_per_params_t bt_per_params;
  struct bb_rf_param_t bb_rf_params;
	
 	unsigned int num_1;
 	unsigned short int num_2;
	unsigned char macaddr[6];
    

	/*Creating a Socket*/
	memset(&bt_per_params, 0, sizeof(bt_per_params_t));
	memset(&bb_rf_params, 0, sizeof(bb_rf_params));
   
  if(argc == NO_OF_ARGS)
  {
    /* Init netlink socket */
    if (rsi_netlink_init() < 0)
    {
      ONEBOX_PRINT ("Netling Socket creation error\n");
      return -1;
    }

    memset (macaddr, 0, 6);
    byteconversion(argv[1], macaddr);

    bt_per_params.enable = 1;

    for( num_1 = 0; num_1 < 6; num_1++ )
    {
      bt_per_params.bt_addr[5-num_1] = macaddr[num_1];
    }
    bt_per_params.pkt_type = atoi(argv[2]);
    bt_per_params.pkt_length = atoi(argv[3]);
    bt_per_params.edr_indication = atoi(argv[4]);
    bt_per_params.bt_rx_rf_chnl = atoi(argv[5]);
    bt_per_params.bt_tx_rf_chnl = atoi(argv[6]);
    bt_per_params.link_type = atoi(argv[7]);
    bt_per_params.scrambler_seed = atoi(argv[8]);
    bt_per_params.num_pkts = atoi(argv[9]);
    bt_per_params.payload_data_type = atoi(argv[10]);
    bt_per_params.mode = atoi(argv[11]);
    bt_per_params.le_chnl = atoi(argv[12]);
    bt_per_params.tx_pwr_indx = atoi(argv[13]);
    bt_per_params.transmit_mode = atoi(argv[14]);
    bt_per_params.enable_hopping = atoi(argv[15]);
    bt_per_params.ant_select = atoi(argv[16]);

		bb_rf_params.value = TX_STATUS;
		if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER,sizeof(bb_rf_params)) < 0 )
		{
			printf("Unable to perform bt_transmit\n");
			return -1;
		}
		if(recv_data((uint_8 *)&bb_rf_params.Data[0]) < 0)
		{
			printf("======== ALREADY IN TRANSMIT SO STOP TRANSMIT FIRST ============\n");
			return -1;
		}

		bb_rf_params.value = GET_DRV_COEX_MODE;
		if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER,sizeof(bb_rf_params)) < 0 )
		{
			printf("Unable to perform bt_transmit\n");
			return -1;
		}
		
    rc = recv_data((uint_8 *)&bb_rf_params.Data[0]);
    
    if( rc == 1 )
    {
      if( bt_per_params.mode != BT_CLASSIC )
      {
        printf("Invalid le_classic_mode parameter, in Classic oper mode\n");
        return  -1;
      }
      if( bt_per_params.bt_rx_rf_chnl < 0 || bt_per_params.bt_rx_rf_chnl > 79 )
      {
        printf("Invalid rx_channel in CLASSIC oper mode\n");
        return -1;
      }
      if( bt_per_params.bt_tx_rf_chnl < 0 || bt_per_params.bt_tx_rf_chnl > 79 )
      {
        printf("Invalid tx_channel in CLASSIC oper mode\n");
        return -1;
      }
      if((bt_per_params.enable_hopping == 1) && ((bt_per_params.bt_tx_rf_chnl == bt_per_params.bt_rx_rf_chnl)))
      {
        printf("Tx and Rx channels should be different in the fixed hopping\n");
        return -1;
      }
    }
    else if( rc == 2 )
    {
      if( bt_per_params.mode != BT_LE )
      {
        printf("Invalid le_classic_mode parameter, in LE oper mode\n");
        return  -1;
      }
      if( bt_per_params.bt_rx_rf_chnl < 0 || bt_per_params.bt_rx_rf_chnl > 39 )
      {
        printf("Invalid rx_channel in LE opermode\n");
        return -1;
      }
      if( bt_per_params.bt_tx_rf_chnl < 0 || bt_per_params.bt_tx_rf_chnl > 39 )
      {
        printf("Invalid tx_channel in LE oper mode\n");
        return -1;
      }
    }
    else
    {
      printf("Invalid Driver mode\n");
      return -1;
    }
    
    if(!((bt_per_params.enable_hopping >= 0) && (bt_per_params.enable_hopping < 3 )))
    {
      printf("Invalid Hopping Mode\n");
      return -1;
    }
    if(!((bt_per_params.transmit_mode >= 0) && (bt_per_params.transmit_mode < 2 )))
    {
      printf("Invalid transmit Mode\n");
      return -1;
    }

    if(!((bt_per_params.edr_indication >= 1) && (bt_per_params.edr_indication < 4 )))
    {
      printf("Invalid BT_EDR Mode\n");
      return -1;
    }
    if(!((bt_per_params.link_type >= 0) && (bt_per_params.link_type < 3 )))
    {
      printf("Invalid link type Mode\n");
      return -1;
    }

    printf("the packet type is %d \n",bt_per_params.pkt_type);
    printf("the packet length is %d \n",bt_per_params.pkt_length);

    bb_rf_params.value = BT_PER_TRANSMIT;
    bb_rf_params.no_of_values = sizeof(bt_per_params_t);
    memcpy(&bb_rf_params.Data[0], &bt_per_params, sizeof(bt_per_params_t));

    usleep(500000);

    if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER,sizeof(bb_rf_params)) < 0 )
    {
      printf("Unable to perform bt_transmit\n");
      return -1;
    }
    printf("======== SUCCESS ============\n");

    if (rsi_netlink_deinit() < 0)
    {
      ONEBOX_PRINT ("Netling Socket creation error\n");
      return -1;
    }
  }
  else if(argc == STOP_OF_ARGS)
  {
    /* Init netlink socket */
    if (rsi_netlink_init() < 0)
    {
      ONEBOX_PRINT ("Netling Socket creation error\n");
      return -1;
    }

    bt_per_params.enable = 0;
    printf("============= BT_TRANSMIT_STOP ==================");

    bb_rf_params.value = BT_PER_TRANSMIT;
    bb_rf_params.no_of_values = sizeof(bt_per_params_t);
    memcpy(&bb_rf_params.Data[0], &bt_per_params, sizeof(bt_per_params_t));

    usleep(500000);

    if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER, sizeof(bb_rf_params)) < 0 )
    {
      printf("Unable to perform bt_transmit\n");
      return -1;
    }
    printf("======== SUCCESS ============\n");

  if (rsi_netlink_deinit() < 0)
  {
    ONEBOX_PRINT ("Netling Socket creation error\n");
    return -1;
  }

  }
#if 1
	else
	{
		printf("\nUSAGE to start transmit: %s <dev_addr> <pkt_type> \n <pkt_length> <bt_edr_mode> <rx_channel_index> <tx_channel_index> <link_type>\n <scrambler_seed> <no_of_packets> <payload_type> <classic_le_mode> <le_channel_type> <tx_power>\n <tx_mode> <hopping_type> <ant_sel>\n",argv[0]);
		return 0;

	}
#endif
	return 0;
}
