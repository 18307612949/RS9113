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
#include <linux/types.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include "onebox_util.h"

#define PER_MODE
//#define PER_BASIC_STATS

int main(int argc, char **argv)
{
	int sockfd;
	int freq;
	int first_time = 1;
	int count = 0, stop_bit = 0;
	int i; 
	unsigned short ch_width = 0;
	FILE *pFile;
	struct iwreq iwr;
	unsigned int valid_channels_5_Ghz[]   = { 36, 40, 44, 48, 52, 56, 60, 64, 100,\
									  		  104, 108, 112, 116, 120, 124, 128, 132, 136,\
									          140, 149, 153, 157, 161, 165 
											};
	unsigned int valid_channels_5_Ghz_40Mhz[]   = { 38, 42, 46, 50, 54, 58, 62, 102,\
                							  		106, 110, 114, 118, 122, 126, 130, 134, 138,\
									                151, 155, 159, 163 
											      };
	unsigned int valid_channels_4_9_Ghz_20Mhz[]   = { 184, 188, 192, 196, 8, 12, 16, 0xff, 0x00 };
	unsigned int valid_channels_4_9_Ghz_10Mhz[]   = { 183, 185, 187, 189, 7, 9, 11, 0xff, 0x00 };
	unsigned char enable_40 = 0;
	unsigned char enable_11j = 0;
	unsigned char rate_flags = 0;

	if (argc != 5)
	{
		printf("Onebox dump stats application\n");   
		printf("Usage: %s <file_name> <channel num> <start-stop> <Channel BW>\n", argv[0]);
		printf("\tFile_name   		- File to dump status\n");
		printf("\tChannel num 		- Channel to operate\n");
		printf("\tStart-Stop value\t- 0: Start & 1: Stop \n");
		printf("\tChannel BW  		- 0: 20MHz, 2: Upper 40MHz, 4: Lower 40MHz & 6: Full 40MHz \n");

		return 1;
	}

	/* Creating a Socket */
	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		printf("Unable to create a socket\n");
		return sockfd;
	}

	freq = atoi(argv[2]);
	stop_bit = atoi(argv[3]);
	rate_flags = atoi(argv[4]);
	ch_width = rate_flags & (0x07);
	enable_11j = (rate_flags & BIT(3) );
	
	if( ch_width == BW_U40 || ch_width == BW_L40 || ch_width == BW_F40 )
	{
			enable_40 = 1;
	}

	if(!enable_11j )
	{

			if (freq == 0xFF)
			{
					/* Pass 0xFF so as to skip channel programming */
			}
			else if((freq >= 36 && freq <= 165 && ch_width == BW_20))
			{
					for(i = 0; i < 24; i++)
					{
							if(freq == valid_channels_5_Ghz[i])
							{
									break;
							}
					}
					if(i == 24)
					{
							printf("Invalid Channel issued by user\n");
							exit(0);
					}
			}
			else if((freq >= 36 && freq <= 165 && enable_40))
			{
					for(i = 0; i < 21; i++)
					{
							if(freq == valid_channels_5_Ghz_40Mhz[i])
							{
									break;
							}
					}
					if(i == 21)
					{
							printf("Invalid Channel issued by user\n");
							exit(0);
					}
			}
			else if(!(freq <= 14))
			{
					printf("Invalid Channel issued by user\n");
					exit(0);
			}
	}
	else
	{
			if(ch_width == BW_20)
			{
					for(i = 0; i < sizeof(valid_channels_4_9_Ghz_20Mhz)/sizeof(valid_channels_4_9_Ghz_20Mhz[0]); i++)
					{
							if(freq == valid_channels_4_9_Ghz_20Mhz[i])
							{
									break;
							}
					}
					if(i == sizeof(valid_channels_4_9_Ghz_20Mhz)/sizeof(valid_channels_4_9_Ghz_20Mhz[0]))
					{
							printf("Invalid Channel issued by user\n");
							exit(0);
					}
			}
			else if( ch_width == BW_10 )
			{
					for(i = 0; i < sizeof(valid_channels_4_9_Ghz_10Mhz)/sizeof(valid_channels_4_9_Ghz_10Mhz[0]); i++)
					{
							if(freq == valid_channels_4_9_Ghz_10Mhz[i])
							{
									break;
							}
					}
					if(i == sizeof(valid_channels_4_9_Ghz_10Mhz)/sizeof(valid_channels_4_9_Ghz_10Mhz[0]))
					{
							printf("Invalid Channel issued by user\n");
							exit(0);
					}
			}
			else if( ch_width == BW_5 )
			{
						printf("5MHz BW is not supported\n");
						exit(0);
			}
			else
			{
					printf("Invalid BW Configuration\n");
					exit(0);
			}
	}

	pFile = fopen(argv[1], "w");
	if(pFile == NULL)
	{
		printf("Unable to create a file\n");
		return -1;
	}

	while(1)
	{
		per_stats *sta_info = malloc(sizeof(per_stats));
#if 0
		if(sleep(1)!=0)
		{
			printf("Unable to sleep\n");            
			free(sta_info);        
			break;
		}
#endif
		memset(&iwr, 0, sizeof(iwr));
		/*Indicates that it is receive*/
		iwr.u.data.flags = (unsigned short)PER_RECEIVE; 
		strncpy(iwr.ifr_name, "rpine0", 7);
		iwr.u.data.pointer = sta_info;
		iwr.u.data.length = sizeof(per_stats);
		iwr.u.data.flags |= (unsigned short)freq << 8;
		*(unsigned short *)iwr.u.data.pointer = (unsigned short)rate_flags;

		if (stop_bit)
		{
			iwr.u.data.flags |= (unsigned short)PER_RCV_STOP;
		}
		if(ioctl(sockfd, ONEBOX_HOST_IOCTL, &iwr) < 0)
		{
			printf("Unable to issue ioctl\n");
			free(sta_info);
			break;
		}
		else
		{
			freq = 0;
			if (stop_bit)
			{
				printf(" RECEIVE STOPPED \n");
				break;
			} 
			if(first_time)
			{
				first_time = 0;
				free(sta_info);
				fflush(pFile);
				continue;
			}
			if((count % 20) == 0)
			{
#ifdef PER_MODE
#ifdef PER_BASIC_STATS
				printf(" %8s %8s %8s %8s\n",
						"crc_pass","crc_fail","false_cca","cal_rssi");
				fprintf(pFile, "%8s %8s %8s %8s\n",
						"crc_pass","crc_fail","false_cca","cal_rssi");
#else
				printf("%12s %12s %12s %12s %12s %12s %12s %12s\n","crc_pass","crc_fail","cca_stk","cca_not_stk","pkt_abort","fls_rx_start","false_cca","cal_rssi");
				fprintf(pFile,"\n%12s %12s %12s %12s %12s %12s %12s %12s\n","crc_pass","crc_fail","cca_stk","cca_not_stk","pkt_abort","fls_rx_start","false_cca","cal_rssi");
#endif
			}
#ifdef PER_BASIC_STATS
			printf("%7d %7d %7d %7d\n",
					sta_info->crc_pass,
					sta_info->crc_fail,
					sta_info->cca_idle,
					sta_info->cal_rssi);
			fprintf(pFile, "%7d %7d %7d %7d\n",
					sta_info->crc_pass,
					sta_info->crc_fail,
					sta_info->cca_idle,
					sta_info->cal_rssi);
#else
			printf("%12d %12d %12d %12d %12d %12d %12d %12d\n",
					sta_info->crc_pass,
					sta_info->crc_fail,
					sta_info->cca_stk,
					sta_info->cca_not_stk,
					sta_info->pkt_abort,
					sta_info->fls_rx_start,
					sta_info->cca_idle,
					sta_info->cal_rssi);
			fprintf(pFile,"%12d %12d %12d %12d %12d %12d %12d %12d\n",
					sta_info->crc_pass,
					sta_info->crc_fail,
					sta_info->cca_stk,
					sta_info->cca_not_stk,
					sta_info->pkt_abort,
					sta_info->fls_rx_start,
					sta_info->cca_idle,
					sta_info->cal_rssi);
#endif
#else
				printf(" %20s %8s %10s %10s %10s \n",
						"tx_pkts","retries","pkts_drop","rssi","cons_drops");
				fprintf(pFile, "%20s %10s %10s %10s %10s \n",
						"tx_pkts","retries","pkts_drop","rssi","cons_drops");
			}
			printf("%20d %9d %9d %9d %9d \n",
					sta_info->tx_pkts,
					sta_info->tx_retries,
					sta_info->xretries,
					sta_info->bea_avg_rssi,
					sta_info->max_cons_pkts_dropped);
			fprintf(pFile, "%20d %9d %9d %9d %9d\n",
					sta_info->tx_pkts,
					sta_info->tx_retries,
					sta_info->xretries,
					sta_info->bea_avg_rssi,
					sta_info->max_cons_pkts_dropped
					);
#endif

			++count;    
			free(sta_info);
		}    
		fflush(pFile);
	}
	close(sockfd);
	fclose(pFile);
	return 0;
}
