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
#include <signal.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include "onebox_util.h"

#define PER_MODE

void sniffer_usage(char *argv)
{
	printf("Onebox dump stats application\n");   
	ONEBOX_PRINT
		("Usage: %s base_interface sniffer_mode channel_number channel_BW \n",argv);
	ONEBOX_PRINT
		("Usage: %s base_interface ch_utilization RSSI_threshold start/stop_bit(1 - start,0 - stop) drop_pkt(1 - enable,0 - disable)\n",argv);
	printf("\tChannel BW  		- 0: 20MHz, 2: Upper 40MHz, 4: Lower 40MHz & 6: Full 40MHz \n");

	return ;
}

int sniffer_getcmdnumber (char *command)
{
	if (!strcmp (command, "sniffer_mode"))
	{
		return SNIFFER_MODE;
	}
	if (!strcmp (command, "ch_utilization"))
	{
		return CH_UTILIZATION;
	}
}

int channel_width_validation(unsigned short ch_width)
{

	if((ch_width != 0) && (ch_width != 2) && (ch_width != 4) && (ch_width != 6))
	{
		return 1;
	}
	return 0;

}

int freq_validation(int freq,unsigned short ch_width)
{
	unsigned int valid_channels_5_Ghz_40Mhz[]   = { 38, 42, 46, 50, 54, 58, 62, 102,\
		106, 110, 114, 118, 122, 126, 130, 134, 138,\
			151, 155, 159, 163 };
	unsigned int valid_channels_5_Ghz[]   = { 36, 40, 44, 48, 52, 56, 60, 64, 100,\
		104, 108, 112, 116, 120, 124, 128, 132, 136,\
			140, 149, 153, 157, 161, 165 };
	int i;
	if (freq == 0xFF)
	{
		/* Pass 0xFF so as to skip channel programming */
	}
	else if((freq >= 36 && freq <= 165 && !ch_width))
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
			return 1;
		}
	}
	else if((freq >= 36 && freq <= 165 && ch_width))
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
			return 1;
		}
	}
	else if(!(freq <= 14))
	{
		return 1;
	}
	return 0;

}

int stop_flag;
void handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT %d\n",signo);
		stop_flag = 1;
}

int main(int argc, char **argv)
{
	char ifName[32];
	int cmdNo = -1;
	int sockfd;
	int freq;
	int rssi_threshold;
	int first_time = 0;
	int count = 0, start_bit = 0;
	int drop_pkt = 0;
	int i; 
	unsigned short ch_width = 0;
	unsigned short ch_util = 0;
	struct iwreq iwr;
	per_stats *sta_info = NULL;

	if (argc < 3)
	{
		sniffer_usage(argv[0]);
	}
	else if (argc <= 50)
	{
		/* Get interface name */
		if (strlen(argv[1]) < sizeof(ifName)) {
			strcpy (ifName, argv[1]);
		} else{
			ONEBOX_PRINT("length of given interface name is more than the buffer size\n");	
			return -1;
		}

		cmdNo = sniffer_getcmdnumber (argv[2]);
		//printf("cmd is %d \n",cmdNo);
	}
  
	signal(SIGINT,handler);

	/* Creating a Socket */
	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		printf("Unable to create a socket\n");
		return sockfd;
	}

	switch (cmdNo)
	{
		case SNIFFER_MODE:     //to use in sniffer mode      
			{
				if(argc != 5)
				{
					printf("Invalid number of arguments \n");
					sniffer_usage(argv[0]);
					return;
				}
				freq = atoi(argv[3]);
				ch_width = atoi(argv[4]);
				if(channel_width_validation(ch_width))
				{
					printf("Invalid Channel BW values \n");
					return;
				}
				if(freq_validation(freq,ch_width))
				{
					printf("Invalid Channel issued by user\n");
					return;
				}
				memset(&iwr, 0, sizeof(iwr));
				iwr.u.data.flags = (unsigned short)PER_RECEIVE;                                
				/*Indicates that it is receive*/
				strncpy (iwr.ifr_name, ifName, IFNAMSIZ);
				iwr.u.data.flags |= (unsigned short)freq << 8;
				iwr.u.data.pointer = &ch_width;


				if(ioctl(sockfd, ONEBOX_HOST_IOCTL, &iwr) < 0)
				{
					printf("Unable to issue ioctl\n");
					return -1;
				}

				break;
			}
		case CH_UTILIZATION:     //to use for channel utilization      
			{
				if(argc != 6)
				{
					printf("Invalid number of arguments \n");
					sniffer_usage(argv[0]);
					return;
				}

				rssi_threshold = atoi(argv[3]);
				if(rssi_threshold == 0)
				{
					rssi_threshold = 90;
				}
				if(rssi_threshold < 40 || rssi_threshold > 90)
				{
					printf("Invalid Rssi Threshold should be 40 to 90 \n");
					return;
				}
				start_bit = atoi(argv[4]);
				if(start_bit != 0 && start_bit != 1)
				{
					printf("Invalid (1 - start)/(0 - stop bit)  \n");
					return;
				}
				drop_pkt = atoi(argv[5]);
				if(drop_pkt != 0 && drop_pkt != 1)
				{
					printf("Invalid (1 - enable)/(0 - disable)  \n");
					return;
				}

				while(1)
				{
					sta_info = malloc(sizeof(per_stats));
					if(stop_flag)
					{
						printf("in check \n");
						start_bit = 0;
					}

					memset(&iwr, 0, sizeof(iwr));
					/*Indicates that it is channel utilization start of stop*/
					if(start_bit == 1)
					{
						if(first_time == 0)
						{
#if 0
							ch_util = drop_pkt & 0xff;
							ch_util = start_bit | (ch_util << 1);
							ch_util = ch_util << 8;
							ch_util |= (rssi_threshold & 0xff);
#endif
							
							ch_util = rssi_threshold;
							ch_util |= ((drop_pkt & 0XFF) << 9);
							ch_util |= (start_bit << 8);
							first_time++;
						
							iwr.u.data.pointer = &ch_util;
							iwr.u.data.flags = CH_UTIL_START;
							strncpy (iwr.ifr_name, ifName, IFNAMSIZ);
							iwr.u.data.length = sizeof(ch_util);
						}
						else
						{
							iwr.u.data.pointer = sta_info;// get sta_info here
							iwr.u.data.length = sizeof(per_stats);
						}
						iwr.u.data.flags = (unsigned short)CH_UTIL_START;
					}
					else                                
					{
						iwr.u.data.flags = (unsigned short)CH_UTIL_STOP;
						ch_util = rssi_threshold;
						ch_util |= ((drop_pkt & 0XFF) << 9);
						ch_util |= (start_bit << 8);
						iwr.u.data.pointer = &ch_util;
					}
					strncpy (iwr.ifr_name, ifName, IFNAMSIZ);
					//printf("&&&&777 %x \n", ch_util);
					if(ioctl(sockfd, ONEBOX_HOST_IOCTL, &iwr) < 0)
					{
						printf("Unable to issue ioctl \n");
						free(sta_info);
						return -1;
					}

					if(start_bit == 1)
					{
						if((count % 20) == 0)
						{
							printf(" TOT_UTIL(%)\t");
							printf(" TOT_BITS(Kbps)\t");
							printf(" CRC_FAILS\t");
							printf(" CRC_PASS\n");
							//printf(" NON_WIFI(%)\n");
						}
						if(count != 0 && count != 1)
						{
							printf(" %7.3f %%\t",(((float)sta_info->utilization)/((float)sta_info->interval_duration))*100);//(tot_on_air_occupancy / tot_time ));
							printf(" %8d \t",(int)((sta_info->tot_bytes*8)/1000));//(tot_on_air_occupancy / tot_time ));
							printf(" %6d \t",(int)sta_info->crc_fail);//(tot_on_air_occupancy / tot_time ));
							printf(" %6d \n",(int)sta_info->crc_pass);//(tot_on_air_occupancy / tot_time ));
							//printf(" %10.3f \n",((float)sta_info->cca_stk*20/((float)sta_info->interval_duration))*100);//(tot_on_air_occupancy / tot_time ));
						}
					}
					else
					{
						free(sta_info);
						break;
					}
					free(sta_info);
					count++;
				}

				break;
			}
		default:
			printf("Invalid Command\n");
			break;
	}

	close(sockfd);
	return 0;
}
