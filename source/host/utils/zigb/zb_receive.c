/**
 * @file receive.c 
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
 * This file  prints the Per frames received from the PPE 
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include "zb_util.h"



#endif
int main(int argc, char **argv)
{
	int sockfd;
	int freq;
	int count = 0;
	int i; 
	FILE *pFile;
	struct iwreq iwr;
	struct bb_rf_param_t bb_rf_params;

	if (argc == 3)
	{
		if (rsi_netlink_init() < 0)
		{
			ONEBOX_PRINT ("Netlink Socket creation error\n");
			return -1;
		}

		pFile = fopen(argv[1], "w");
		if(pFile == NULL)
		{
			printf("Unable to create a file\n");
			return -1;
		}

		freq = atoi(argv[2]);

		if (freq)
		{
			if ((freq < 11) || (freq > 26))
			{
				printf("#### Invalid channel number #####\n");
				exit(0);
			}

#if 0
			/* Setting Channel */
			memset(&iwr, 0, sizeof(iwr));
			iwr.u.data.flags = (unsigned short)PER_SET_CHANNEL;                                
			strncpy(iwr.ifr_name, "zigbee0", IFNAMSIZ);
			iwr.u.data.length = sizeof(per_stats);
			iwr.u.data.flags |= (unsigned short)freq << 8;

			if(ioctl(sockfd, RSI_HOST_IOCTL, &iwr) < 0)
			{
				printf("Unable to set channel\n");
				exit(0);
			}
			else
			{
				printf("channel has been set\n");
			}
#endif
		}

		/* Stats Request */
		while(1)
		{
			unsigned char *sta_info = malloc(sizeof(per_stats));
			if(sleep(1)!=0)
			{
				printf("Unable to sleep\n");            
				free(sta_info);        
				break;
			}
			bb_rf_params.value = ZIGB_PER_RECEIVE;
			bb_rf_params.no_of_values = freq;  /* passing channel here */

			if( rsi_send_to_drv((uint_8 *)&bb_rf_params, ZB_PER, sizeof(bb_rf_params)) < 0 )
			{
				printf("Unable to perform bt_transmit\n");
				return -1;
			}
			else
			{
				if(recv_data((uint_8*)sta_info) < 0 )
				{
					free(sta_info);
					return -1;
				}
				if((count % 20) == 0)
				{
					printf("%12s %12s %12s %12s %12s %12s %12s %12s %12s %12s \n","crc_pass","crc_fail","pkt_abort","tx_pkt","cca_idle","rx_start_stk","rx_cca_stk","fifo_occupy","phy_abort","rx_pkt_tout");
					fprintf(pFile,"%12s %12s %12s %12s %12s %12s %12s %12s %12s %12s\n","crc_pass","crc_fail","pkt_abort","tx_pkt","cca_idle","rx_start_stk","rx_cca_stk","fifo_occupy","phy_abort","rx_pkt_tout");
				}
				printf("%12d %12d %12d %12d %12d %12d %12d %12d %12d %12d\n",
						*(unsigned short *)&sta_info[0],
						*(unsigned short *)&sta_info[2],
						*(unsigned short *)&sta_info[4],
						*(unsigned short *)&sta_info[6],
						*(unsigned short *)&sta_info[8],
						*(unsigned short *)&sta_info[10],
						*(unsigned short *)&sta_info[12],
						*(unsigned short *)&sta_info[14],
						*(unsigned short *)&sta_info[16],
						*(unsigned short *)&sta_info[18]);
				fprintf(pFile,"%12d %12d %12d %12d %12d %12d %12d %12d %12d %12d\n",
						*(unsigned short *)&sta_info[0],
						*(unsigned short *)&sta_info[2],
						*(unsigned short *)&sta_info[4],
						*(unsigned short *)&sta_info[6],
						*(unsigned short *)&sta_info[8],
						*(unsigned short *)&sta_info[10],
						*(unsigned short *)&sta_info[12],
						*(unsigned short *)&sta_info[14],
						*(unsigned short *)&sta_info[16],
						*(unsigned short *)&sta_info[18]);
				++count;    
				free(sta_info);
			}    
			fflush(pFile);
		}
		close(sockfd);
		fclose(pFile);
	}
	else
	{
		printf("zigbee dump stats application\n");   
		printf("Usage: %s <file_name> <channel num> \n", argv[0]);
		printf("\tFile_name   		- File to dump status\n");
		printf("\tChannel num 		- Channel to operate\n");
		return -1;

	}
	return 0;
}
