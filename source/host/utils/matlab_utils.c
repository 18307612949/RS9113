/**
 * @file matlab_utils.c 
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
 * File for generating the onebox utility. This utility is used for
 * creating the Virtual access point(VAP), deleting the VAP, setting wmm params 
 * printing various vap stats and autorate stats etc
 */


#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <linux/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include<netinet/in.h>
#include "onebox_util.h"

int sockfd;
int conn_socket;
int client_sock;
struct iwreq iwr;

int_32 rf_read(uint_32, uint_32, uint_32, uint_32 *);
int_32 rf_write(uint_32, uint_32, uint_32, uint_32 *);
int_32 ulp_read(uint_32, uint_32, uint_32, uint_32 *);
int_32 ulp_write(uint_32, uint_32, uint_32, uint_32 *);
int_32 bb_read(uint_32, uint_32, uint_32);
int_32 bb_write(uint_32, uint_32, uint_32 *);
int_32 lmac_rf_lpbk_m2(uint_32, uint_32 );
int_32 lmac_rf_lpbk_m3(uint_32, uint_32 );
int_32 buffer_read(uint_32 *, uint_32, uint_32);
int_32 buffer_write(uint_32 *, uint_32, uint_32);
int_32 read_stats(uint_32 *,uint_32);
int_32 rf_reset(uint_32 ,uint_32);
int_32 soc_reg_access(uint_32 , uint_32, uint_32);
int_32 set_endpoint(uint_16 type);
int_32 tx_packet(uint_32 *,uint_32);
int_32 calib_flashing(uint_32 *, uint_16, uint_16);

uint_32 BUFFER[5 *1024];

ONEBOX_STATUS onebox_process_packet(uint_8 *, uint_32 ,struct sockaddr_in *, int);
ONEBOX_STATUS onebox_process_packet1(uint_8 *packet, uint_32 packet_len); 
struct bb_rf_param_t  bb_rf_params;
void onebox_hexdump(uint_8 *pkt, uint_16 len)
{
	uint_16 cnt;

	for(cnt=0; cnt<len; cnt++) {
		if (!(cnt%16))
			printf("\n");
		printf("%02x ", pkt[cnt]);
	}
	printf("\n");
}

void onebox_dump(uint_8 *pkt, uint_16 len)
{
	uint_16 cnt;

	for(cnt=0; cnt<len; cnt++) {
		if (!(cnt%16))
			printf("\n");
		printf("%02x ", pkt[cnt]);
	}
	printf("\n");
}

uint_32 onebox_open_interface(void)
{
	FUNCTION_ENTRY();

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);    
	if (sockfd < 0)
	{
		printf("Unable to create a socket\n");
		return sockfd;
	}
	FUNCTION_EXIT();
	return 0;
}

ONEBOX_STATUS onebox_send_packet(uint_16 *packet, uint_16 length)
{
	uint_8 *server_name = DESTN_IP;
	uint_16 send_socket;
	uint_16 swap,i;
	struct sockaddr_in  server;
	ssize_t ret_val;

	server.sin_addr.s_addr = inet_addr(server_name);
	server.sin_family      = AF_INET;
	server.sin_port        = htons(DESTN_PORT_NUM);
	printf(" packet length = %x\n",length);
#ifdef UDP_CONN
	send_socket = socket(AF_INET, SOCK_DGRAM, 0);
#else
	send_socket = socket(AF_INET, SOCK_STREAM, 0);
#endif

#ifdef UDP_CONN
	ret_val = sendto(send_socket, packet, length, 0, 
		  	 (struct sockaddr *)&server, sizeof(server));
#else
	ret_val = send(client_sock, packet, length, 0);
#endif

	if (ret_val == -1)
		printf("Unable to send\n");
	close(send_socket);

	return 0;
}

ONEBOX_STATUS onebox_process_packet(uint_8 *packet, uint_32 packet_len, 
		struct sockaddr_in *client_addr, int client_len)
{
	uint_16 type, ii, jj,i,j;
	uint_16 addr=0, len=0;
	struct iwreq wrq;
	char ifName[10]="rpine0";
	uint_32 temp_len,swap,swap1;
	uint_8 frag_cnt=1;
	uint_16 offset=0, data_len=0;
	uint_32 bb_addr, bb_len, soft_reset = 0;
	uint_32 tx_count;
	uint_32 stats_count;
	uint_32 bb_val,no_of_valid_bits,delay,no_of_addr;
	uint_32 rf_addr, rf_addr1, vals_per_reg, num_of_regs,rf_len; 
	uint_32 lmac_addr = 0, lmac_data = 0;
	uint_32 RCV_BUFFER[5];
	uint_8 arg;
	FUNCTION_ENTRY(); 

	/* Check if packet is invalid */
	if (packet_len <= 0) {
		printf("Invalid length\n");
		goto fail;
	}

	/* Get the command type */ 
	type = ntohl(*(uint_32*)&packet[0]);

	/* For Register read/write commands, take the address and
	   length from the packet */ 

	/* Process individiual packets */
	switch (type) {
		case BB_READ:
			printf("===> BB Read <====\n");
			soft_reset  = ntohl(*(uint_32*)&packet[4]);
			bb_addr = ntohl(*(uint_32*)&packet[8]);
			bb_len  = ntohl(*(uint_32*)&packet[12]);
			printf(" soft_reset is %d :\n",soft_reset);
			printf(" BB_addr is %x :\n",bb_addr);
			printf(" BB_len is %d :\n",bb_len);
			bb_read(bb_addr, bb_len, soft_reset);
			break;

		case BB_WRITE:
			printf("===> BB Write <===\n");
			soft_reset  = ntohl(*(uint_32*)&packet[4]);
			bb_len = ntohl(*(uint_32*)&packet[8]);
			bb_addr = ntohl(*(uint_32*)&packet[12]);
			bb_val = ntohl(*(uint_32*)&packet[16]);
			printf(" soft_reset is %x :\n",soft_reset);
			printf(" BB_len is %x :\n",bb_len);
			printf(" BB_addr is %x :\n",bb_addr);
			printf(" BB_val is %x :\n",bb_val);
			for(ii=8,jj=0;jj<bb_len*2;ii=ii+4,jj++)
			{
				BUFFER[jj] = ntohl(*(uint_32*)&packet[ii+4]);
				printf(" BBffer[%d] is %8x :\n",jj,BUFFER[jj]);
			}
#if 0
			for(jj=0,ii=0;jj<bb_len*2;jj+=2,ii++)
				printf("Address[%d] = %08x Data = %08x \n",ii,BUFFER[jj],BUFFER[jj+1]);
#endif
			bb_write(bb_len, soft_reset, BUFFER);
			break;

		case RF_READ:
			printf("===> RF Read <===\n");
			soft_reset  = ntohl(*(uint_32*)&packet[4]);
			vals_per_reg = ntohl(*(uint_32*)&packet[8]);
			num_of_regs = ntohl(*(uint_32*)&packet[12]);
			rf_len = (num_of_regs * vals_per_reg);
			printf(" soft_reset is= %d, values_per_reg= %d, num_of_regs= %d, total rf_len= %d :\n"
					,soft_reset,vals_per_reg,num_of_regs,rf_len);
			for(ii=12, jj=0;ii<rf_len*4+12;ii+=4,jj++)
			{
				BUFFER[jj] = ntohl(*(uint_32*)&packet[ii+4]);
			}
			rf_read(soft_reset, num_of_regs, vals_per_reg, BUFFER);
			break;

		case RF_WRITE:
			printf("===> RF Write <===\n");
			soft_reset  = ntohl(*(uint_32*)&packet[4]);
			vals_per_reg = ntohl(*(uint_32*)&packet[8]);
			num_of_regs = ntohl(*(uint_32*)&packet[12]);
			rf_len = (num_of_regs * vals_per_reg);
			printf(" soft_reset is= %d, values_per_reg= %d, num_of_regs= %d, total rf_len= %d :\n"
					,soft_reset,vals_per_reg,num_of_regs,rf_len);
			for(ii=12, jj=0;ii<rf_len*4+12;ii+=4,jj++)
			{
				BUFFER[jj] = ntohl(*(uint_32*)&packet[ii+4]);
			}
#if 0
			for(jj=0,i=0;jj<rf_len;jj+=5,i++)
				printf("valid_bits= %d, Data2[%d] = %08x ,Data1 = %08x ,Data0 = %08x ,Delay = %d\n",BUFFER[jj],
						i,BUFFER[jj+1],BUFFER[jj+2],BUFFER[jj+3],BUFFER[jj+4]);
#endif
			rf_write(soft_reset, num_of_regs, vals_per_reg, BUFFER);
			break;


		case ULP_READ:
			printf("===> ULP Read <===\n");
			soft_reset  = ntohl(*(uint_32*)&packet[4]);
			vals_per_reg = ntohl(*(uint_32*)&packet[8]);
			num_of_regs = ntohl(*(uint_32*)&packet[12]);
			rf_len = (num_of_regs * vals_per_reg);
			printf(" soft_reset is= %d, values_per_reg= %d, num_of_regs= %d, total rf_len= %d :\n"
					,soft_reset,vals_per_reg,num_of_regs,rf_len);
			for(ii=12, jj=0;jj<rf_len;ii+=4,jj++)
				//                     for(ii=12, jj=0;ii<rf_len*4+12;ii+=4,jj++)
			{
				BUFFER[jj] = ntohl(*(uint_32*)&packet[ii+4]);
			}
#if 0
			for(jj=0;jj<rf_len;jj+=5)
				printf("Address = %08x, %08x ,Data = %08x,  %08x ,Delay = %d\n",BUFFER[jj],
						BUFFER[jj+1],BUFFER[jj+2],BUFFER[jj+3],BUFFER[jj+4]);
#endif
			ulp_read(soft_reset, num_of_regs, vals_per_reg, BUFFER);
			break;

		case ULP_WRITE:
			printf("===> ULP Write <===\n");
			soft_reset  = ntohl(*(uint_32*)&packet[4]);
			vals_per_reg = ntohl(*(uint_32*)&packet[8]);
			num_of_regs = ntohl(*(uint_32*)&packet[12]);
			rf_len = (num_of_regs * vals_per_reg);
			printf(" soft_reset is= %d, values_per_reg= %d, num_of_regs= %d, total rf_len= %d :\n"
					,soft_reset,vals_per_reg,num_of_regs,rf_len);
			for(ii=12, jj=0;ii<rf_len*4+12;ii+=4,jj++)
			{
				BUFFER[jj] = ntohl(*(uint_32*)&packet[ii+4]);
			}
#if 0
			for(jj=0;jj<rf_len;jj+=5)
				printf("valid_bits= %d, Data2[%d] = %08x ,Data1 = %08x ,Data0 = %08x ,Delay = %d\n",BUFFER[jj],
						jj,BUFFER[jj+1],BUFFER[jj+2],BUFFER[jj+3],BUFFER[jj+4]);
#endif
			ulp_write(soft_reset, num_of_regs, vals_per_reg, BUFFER);
			break;

		case BUFFER_READ:
			printf("===> Buffer Read <===\n");
			soft_reset  = ntohl(*(uint_32*)&packet[4]);
			no_of_addr = ntohl(*(uint_32*)&packet[20]);
			printf(" soft_reset is= %d, num_of_sets= %d \n",soft_reset,no_of_addr);
			BUFFER[0] = ntohl(*(uint_32*)&packet[8]);
			BUFFER[1] = ntohl(*(uint_32*)&packet[12]);
			BUFFER[2] = ntohl(*(uint_32*)&packet[16]);
			printf(" X_reg_addr= %08x, Y_reg_addr= %08x, Z_reg_addr= %08x \n",BUFFER[0],BUFFER[1],BUFFER[2]);
			for(ii=20,jj=3;ii<=no_of_addr*12+20;ii=ii+4,jj++)
			{
				BUFFER[jj] = ntohl(*(uint_32*)&packet[ii+4]);
			}

#if 1
			for(jj=3;jj<no_of_addr*3+3;jj+=3)
				printf(" XData2[%d] = %08x ,YData= %08x ,ZData= %08x \n",(jj-3),BUFFER[jj],BUFFER[jj+1],BUFFER[jj+2]);
#endif

			buffer_read(BUFFER, no_of_addr, soft_reset);  
			break;
		case BUFFER_WRITE:
			printf("===> Buffer Write <===\n");
			soft_reset  = ntohl(*(uint_32*)&packet[4]);
			no_of_addr = ntohl(*(uint_32*)&packet[20]);
			printf(" soft_reset is= %d, num_of_sets= %d \n",soft_reset,no_of_addr);
			BUFFER[0] = ntohl(*(uint_32*)&packet[8]);
			BUFFER[1] = ntohl(*(uint_32*)&packet[12]);
			BUFFER[2] = ntohl(*(uint_32*)&packet[16]);
			printf(" X_reg_addr= %08x, Y_reg_addr= %08x, Z_reg_addr= %08x \n",BUFFER[0],BUFFER[1],BUFFER[2]);
			for(ii=20,jj=3;ii<=no_of_addr*12+20;ii=ii+4,jj++)
			{
				BUFFER[jj] = ntohl(*(uint_32*)&packet[ii+4]);
			}

#if 1
			for(jj=3;jj<no_of_addr*3+3;jj+=3)
				printf(" XData2[%d] = %08x ,YData= %08x ,ZData= %08x ,Delay = %d\n",(jj-3),BUFFER[jj],BUFFER[jj+1],BUFFER[jj+2],BUFFER[jj+3]);
#endif

			buffer_write(BUFFER, no_of_addr, soft_reset);

			break;
		case RX_STATS:
			{
				printf("===> RX_STATS <===\n");
				arg = ntohl(*(uint_32*)&packet[4]);

				memset(RCV_BUFFER, 0 ,arg);

				for ( i =0 ; i<arg; i++)
					RCV_BUFFER[i] = ntohl(*(uint_32*)&packet[ (i*4) + 8 ]);
				
				read_stats(RCV_BUFFER,arg);
				
			}
			break;

		case LMAC_RF_M3:  //reading LMAC memory in RF LOOPBACK MODE
			printf("===> LMAC RF METHOD 3 <===\n");
			soft_reset  = ntohl(*(uint_32*)&packet[4]);
			bb_len = ntohl(*(uint_32*)&packet[8]);
			printf(" soft_reset is %x :\n",soft_reset);
			printf(" Write_len is %x :\n",bb_len);
//			for(ii=8,jj=0;jj<bb_len*2;ii=ii+4,jj++)
			{
//				BUFFER[jj] = ntohl(*(uint_32*)&packet[ii+4]);
//				printf(" BBffer[%d] is %8x :\n",jj,BUFFER[jj]);
			}
			//lmac_rf_lpbk_write(bb_len, soft_reset, BUFFER);
			lmac_rf_lpbk_m3(bb_len, soft_reset);
			break;

		case LMAC_RF_M2:  //reading LMAC memory in RF LOOPBACK MODE
			printf("===> LMAC RF METHOD 2 <===\n");
			soft_reset  = ntohl(*(uint_32*)&packet[4]);
			bb_len = ntohl(*(uint_32*)&packet[8]);
			printf(" soft_reset is %x :\n",soft_reset);
			printf(" read_len is %x :\n",bb_len);
			lmac_rf_lpbk_m2(bb_len, soft_reset);
			break;

		case RF_RESET:
			{
				printf("===> RF_RESET <===\n");
				type = (uint_16)(ntohl(*(uint_32*)&packet[4]));
				delay = ntohl(*(uint_32*)&packet[8]);
				printf("type :0x%x, delay : 0x%x\n",type, delay);
				rf_reset((uint_32)type,delay);
			}
			break;

		case ENDPOINT:
			{
				printf("===> ENDPOINT <===\n");
				type = (uint_16)(ntohl(*(uint_32*)&packet[4]));
				printf("type :0x%x, \n",type );
				set_endpoint(type);
			}
			break;
		case SOC_REG_WRITE:
		case SOC_REG_READ:
			{
				printf("===> SOC_REG_READ/WRITE <===\n");
				lmac_addr = ntohl(*(uint_32*)&packet[4]);
				lmac_data = ntohl(*(uint_32*)&packet[8]);
				printf("type : %d , LMAC addr :0x%x, data : 0x%x\n",type, lmac_addr, lmac_data);
				soc_reg_access((uint_32)type, lmac_addr, lmac_data);
			}
			break;
		case TX_STATS:
			{
				printf("===> TX_STATS <===\n");
				tx_count = ntohl(*(uint_32 *)&packet[4]); //no of arguments
				memset(BUFFER, 0, tx_count);
					
				for( i=0, j=0 ; i < tx_count; i++ )
				{
					BUFFER[i] = ntohl(*(uint_32 *)&packet[j + 8]);
					j = j+4;
				}
				tx_packet(BUFFER,tx_count);
				
			}
			break;
		case CALIB_FLASHING:
			printf("===> CALIB_FLASHING <===\n");
			len = ntohl(*(uint_32 *)&packet[4]); //no of arguments
			type = ntohl(*(uint_32 *)&packet[8]); //no of arguments
			printf("len is %d, type %d", len, type);
			memset(BUFFER, 0, len);

			for( i=0, j=0 ; i < len; i++ )
			{
				BUFFER[i] = ntohl(*(uint_32 *)&packet[j + 12]);
				j = j+4;
			}
			calib_flashing(BUFFER, len, type);
			break;

		default:
			printf("### Unknow command %d ###\n", type);
			packet[0] = type;
			onebox_send_packet((uint_16 *)packet,2);
	}

	return ONEBOX_STATUS_SUCCESS;
fail:
	return ONEBOX_STATUS_FAILURE;
}

int_32 buffer_write(uint_32 *BUFFER, uint_32 no_of_addr, uint_32 soft_rst)
{
	uint_16 val, i, ii, j = 3,index=0,k=0, buf_len;
	uint_8 blocks, count,count1, write_block = 27;
	struct bb_rf_param_t  bb_rf_params;

	val = (uint_16)no_of_addr;
	printf("Total no regs are = %d :\n",val);
	blocks = (no_of_addr/(write_block)); 
	count = (no_of_addr%(write_block)); 
	for ( i=0; i<=blocks ; i++)
	{
		if (i == blocks)
		{
			count1 = count;  
		} 
		else 
		{
			count1 = write_block;  
		}      
		index = j;
		printf("index = %d :\n",index);
		bb_rf_params.Data[1] = *(uint_16 *)&BUFFER[0];
		bb_rf_params.Data[2] = *(uint_16 *)&BUFFER[1];
		bb_rf_params.Data[3] = *(uint_16 *)&BUFFER[2];
		for (ii=4,k=1 ; ii<(count1*3 + 3);ii+=3,k++, j+=3)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+1];
			bb_rf_params.Data[ii+2] = ((*(uint_16 *)&BUFFER[j+2]) | 0x40) ;

			printf("X Data[%d] = %08x ,YData = %08x ,ZData = %08x \n",k,bb_rf_params.Data[ii],bb_rf_params.Data[ii+1],bb_rf_params.Data[ii+2]);
		}
		buf_len = count1;
		printf("Total block = %d :\n",buf_len);
		strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
		bb_rf_params.value = 7;
		bb_rf_params.no_of_values = buf_len;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
		iwr.u.data.pointer = &bb_rf_params;
		iwr.u.data.length = (buf_len); 
		if(ioctl(sockfd,ONEBOX_SET_BB_RF,&iwr)<0)
		{  
			printf("Error in buffer writing \n");
		}  
		else
		{ 
			printf("Buffer Writing Successful:\n");
		}
	}  
	return 0;

}

int_32 tx_packet(uint_32 *buf,uint_32 tx_cnt)
{
	unsigned int valid_channels_5_Ghz[]   = { 36, 40, 44, 48, 52, 56, 60, 64, 100,\
		104, 108, 112, 116, 120, 124, 128, 132, 136,\
			140, 149, 153, 157, 161, 165 
	};
	unsigned int valid_channels_5_Ghz_40Mhz[]   = { 38, 42, 46, 50, 54, 58, 62, 102,\
		106, 110, 114, 118, 122, 126, 130, 134, 138,\
			151, 155, 159, 163 
	};
	unsigned int valid_channels_4_9_Ghz_20Mhz[]   = { 184, 188, 192, 196, 8, 12, 16,0xff, 0x00 };
	unsigned int valid_channels_4_9_Ghz_10Mhz[]   = { 183, 184, 185, 187, 188, 189, 7, 8, 9, 11,0xff, 0x00 };
	unsigned char enable_40 = 0;
	unsigned char enable_11j = 0;
	int chan_number;
        int ii=0 ,i;
        unsigned short tmp_rate;
        per_params_t per_params;
	unsigned char rate_flags = 0;


        if( tx_cnt == 1 )
        {	
		printf("TX STOP\n");
                
		if((buf[0] == 0) || (buf[1] == 1))
		{
			per_params.mode = buf[0];
			per_params.enable = 0;
			memset(&iwr, 0, sizeof(iwr));
			strncpy(iwr.ifr_name, "rpine0", 6);
			 /*Indicates that it is transmission*/
			iwr.u.data.flags = (unsigned short)PER_TRANSMIT;    
			iwr.u.data.pointer = (unsigned char *)&per_params;
			iwr.u.data.length  = sizeof(per_params);

			if(ioctl(sockfd, ONEBOX_HOST_IOCTL, &iwr) < 0)
			{
				//perror(argv[0]);
				printf("&&Please ensure Burst or Continuous Mode is running\n");
			}
			else
			{
				printf("Tx Stopped\n");
			}
		}
		else
		{
			printf("Please enter either 0 or 1 as an argument, instead of %d to stop..\n",buf[0]);
		}
        }
        else if(tx_cnt == 11)
        {
        	printf("TX START MODE\n");
		
		memset(&iwr, 0, sizeof(iwr));
		strncpy(iwr.ifr_name, "rpine0", 7); /* 7 so that a terminating null byte is automatically copied by strncpy */

		 per_params.enable = 1;
		 per_params.power= buf[0];
       		 tmp_rate = buf[1];
       		 
		if( tmp_rate == 1 )
			per_params.rate = RSI_RATE_1;
		else if( tmp_rate == 2 )
			per_params.rate = RSI_RATE_2;
		else if( tmp_rate == 5 )
			per_params.rate = RSI_RATE_5_5; 
		else if( tmp_rate == 11 )
			per_params.rate = RSI_RATE_11;
		else if( tmp_rate == 6)
			per_params.rate = RSI_RATE_6;
		else if( tmp_rate == 9)
			per_params.rate = RSI_RATE_9;
		else if( tmp_rate == 12)
			per_params.rate = RSI_RATE_12;
		else if( tmp_rate == 18 )
			per_params.rate = RSI_RATE_18;
		else if( tmp_rate == 24 )
			per_params.rate = RSI_RATE_24;
		else if( tmp_rate == 36 )
			per_params.rate = RSI_RATE_36;
		else if( tmp_rate == 48 )
			per_params.rate = RSI_RATE_48;
		else if( tmp_rate == 54 )
			per_params.rate = RSI_RATE_54;
		else if( tmp_rate == RSI_RATE_MCS0 )
			per_params.rate = RSI_RATE_MCS0;
		else if( tmp_rate == RSI_RATE_MCS1 )
			per_params.rate = RSI_RATE_MCS1;
		else if( tmp_rate == RSI_RATE_MCS2 )
			per_params.rate = RSI_RATE_MCS2;
		else if( tmp_rate == RSI_RATE_MCS3)
			per_params.rate = RSI_RATE_MCS3;
		else if( tmp_rate == RSI_RATE_MCS4)
			per_params.rate = RSI_RATE_MCS4;
		else if( tmp_rate == RSI_RATE_MCS5 )
			per_params.rate = RSI_RATE_MCS5;
		else if( tmp_rate == RSI_RATE_MCS6)
			per_params.rate = RSI_RATE_MCS6;
		else if( tmp_rate == RSI_RATE_MCS7 )
			per_params.rate = RSI_RATE_MCS7;
		else
			per_params.rate = RSI_RATE_1;

		per_params.pkt_length = buf[2];

		if( buf[3] == 1 || buf[3] == 0)
		{
			per_params.mode = buf[3];
		}
		else
		{
			per_params.mode = 0;
		}

		chan_number = buf[4];

		rate_flags = buf[6];
		per_params.rate_flags = rate_flags;
		per_params.per_ch_bw= (rate_flags >> 2) & 0x07;
		enable_11j = (rate_flags & BIT(5));
		per_params.enable_11j = enable_11j;
		per_params.aggr_enable  = buf[7];

		per_params.aggr_count   = (per_params.pkt_length/PER_AGGR_LIMIT_PER_PKT);
		if((per_params.pkt_length - (per_params.aggr_count * PER_AGGR_LIMIT_PER_PKT)) > 0)
		{	
			per_params.aggr_count++; 
		}
		if(per_params.aggr_count == 1)
		{
			per_params.aggr_enable = 0;
			per_params.aggr_count = 0;
		}

      		 per_params.no_of_pkts   = buf[8];

		per_params.delay   = buf[9];
		per_params.ctry_region   = buf[10];
		if(per_params.ctry_region == 255) {
		  /*** Remove Me When Updated in Doc and More regions are added*/
		  per_params.ctry_region = 127;
		}
		else if((per_params.ctry_region < 0)  || (per_params.ctry_region > 2))
		{
			printf("Invalid Country region \n");	
    			printf("Valid country regions are : 0- FCC(US), 1- ETSI(Europe), 2-JP (japan), 255-World\n");
			return -1;
		}	

		if(per_params.pkt_length > 1536 && per_params.aggr_enable ==0 )
		{
			printf("Invalid length,Give the length <= 1536 \n");
			exit(0);
		}
		if((per_params.pkt_length > 30000) && (per_params.aggr_enable))
		{
			printf("Cant aggregate,Give the length <= 30000 \n");
			exit(0);
		}
		if((per_params.aggr_enable) && !(per_params.rate >= RSI_RATE_MCS0 && per_params.rate <= RSI_RATE_MCS7))
		{
			printf("Cant aggregate,Give 11n rate \n");
			exit(0);
		}
		if( per_params.per_ch_bw == BW_U40 || per_params.per_ch_bw == BW_L40 || per_params.per_ch_bw == BW_F40)
		{
				enable_40 = 1;
		}
		if( enable_11j == 0)
		{
				if (chan_number == 0xFF)
				{
						per_params.channel = chan_number;	
						/* Pass 0xFF so as to skip channel programming */
				}
				else if(chan_number <= 14)
				{
						per_params.channel = chan_number;
				}
				else if((chan_number >= 36 && chan_number <= 165) && per_params.per_ch_bw == BW_20) /* For 20Mhz BW */
				{
						for(i = 0; i < 24; i++)
						{
								if(chan_number == valid_channels_5_Ghz[i])
								{
										per_params.channel = chan_number;
										break;
								}
						}
						if(!(per_params.channel == chan_number))
						{
								printf("Invalid Channel issued by user for 20Mhz BW\n");
                return -1;
						}
				}
				else if((chan_number >= 36 && chan_number <= 165) && enable_40) /* For 20Mhz BW */
				{
						for(i = 0; i < 21; i++)
						{
								if(chan_number == valid_channels_5_Ghz_40Mhz[i])
								{
										per_params.channel = chan_number;
										break;
								}
						}
						if(!(per_params.channel == chan_number))
						{
								printf("Invalid Channel issued by user for 40Mhz BW\n");
								return -1;
						}
				}
				else
				{
						printf("Invalid Channel issued by user\n");
						return -1;
				}
		}
		else
		{
				if(per_params.per_ch_bw == BW_20)
				{
						for(i = 0; i < sizeof(valid_channels_4_9_Ghz_20Mhz)/sizeof(valid_channels_4_9_Ghz_20Mhz[0]); i++)
						{
								if(chan_number == valid_channels_4_9_Ghz_20Mhz[i])
								{
										per_params.channel = chan_number;
										break;
								}
						}
						if(!(per_params.channel == chan_number))
						{
								printf("Invalid Channel issued by user for 20Mhz BW\n");
								return -1;
						}
				}
				else if( per_params.per_ch_bw == BW_10)
				{
						for(i = 0; i < sizeof(valid_channels_4_9_Ghz_10Mhz)/sizeof(valid_channels_4_9_Ghz_10Mhz[0]); i++)
						{
								if(chan_number == valid_channels_4_9_Ghz_10Mhz[i])
								{
										per_params.channel = chan_number;
										break;
								}
						}
						if(!(per_params.channel == chan_number))
						{
								printf("Invalid Channel issued by user for 20Mhz BW\n");
								return -1;
						}
				}
				else if(per_params.per_ch_bw == BW_5 )
				{
						printf("5MHz BW is not supported\n");
						return -1;
				}
				else
				{
						printf("Invalid BW Configuration\n");
						return -1;
				}

		}
		
		printf("\n--Tx TEST CONFIGURATION--\n\n");
		printf("Tx POWER      : %d\n",buf[0]);
		printf("Tx RATE       : %d\n", buf[1]);
		printf("PACKET LENGTH : %d\n",per_params.pkt_length);
		if( per_params.mode == 1)
		{
			printf("Tx MODE       : CONTINUOUS\n");
			per_params.pkt_length = 28;
		}
		else if (per_params.mode == 0)
		{
			printf("Tx MODE       : BURST\n");
		}
		else
		{
			printf("Tx MODE       : CONTINUOUS\n");
		}
		printf("CHANNEL NUM   : %d\n", chan_number);
		printf("RATE_FLAGS    : %d\n", per_params.rate_flags);
		printf("CHAN_WIDTH    : %d\n", per_params.per_ch_bw);
		printf("AGGR_ENABLE   : %d\n", per_params.aggr_enable);
		printf("NO OF PACKETS : %d\n", per_params.no_of_pkts);
		printf("DELAY         : %d\n", per_params.delay);
		
		
		/* Filling the iwreq structure */ 
		memset(&iwr, 0, sizeof(iwr));
		strncpy(iwr.ifr_name, "rpine0", 6);
		
		/*Indicates that it is transmission*/
		iwr.u.data.flags = (unsigned short)PER_TRANSMIT;
		iwr.u.data.pointer = (unsigned char *)&per_params;
		iwr.u.data.length  = sizeof(per_params);
		
		if(ioctl(sockfd, ONEBOX_HOST_IOCTL, &iwr) < 0)
		{
			//perror(argv[0]);
			printf("Please ensure OneBox Driver is running with valid arguments \tor stop existing transmit utility\n");
		}
		else
		{
			printf("Tx Started\n");
		}
	}
        else
        {
		printf("\nUSAGE to start transmit through matlab: type no_of_arguments tx_power rate length tx_mode channel ExtPA-Enable Rate_flags Aggr_enable no_of_packets delay \n");
		printf("\nUSAGE to stop transmit: type no_of_arguments 0/1\n\t****** FIELDS *******");
		printf("\ntx_mode : 0 - Burst , 1 - Continuous mode\n");
		printf("\nRate_flags Bits: Reserved: 6-15   CH_BW: 5-2   GreenField: 1    Short_GI: 0 \n\n");
		return 0;
        }
		return 0;
}

int_32 calib_flashing(uint_32 *buf,uint_16 len, uint_16 type)
{
	FILE  *pFile, *pFile1;
	int   count; 

	pFile = fopen("../release/flash/RS9113_RS8111_calib_values.txt", "w");
	if(pFile == NULL)
	{
		printf("Unable to create a file\n");
		return -1;
	}
	for (count = 0; count < len; count++)
	{
		fprintf(pFile, "0x%x,\n", buf[count]);
//		printf(" buf[%d], 0x%x,\n",count, (unsigned char) buf[count]);
	}
	fclose(pFile);
	printf("File created successfully\n");
	if(!type)
		system("sh ../release/flash/run_calib.sh");
	else
		system("sh ../release/flash/run_calib_wc.sh");
	return 0;
}

#define NO_OF_PARAMS 4
#define SIZE_OF_PARAM 2
int_32 read_stats(uint_32 *packet,uint_32 arg)
{
	
	unsigned int valid_channels_5_Ghz[]   = { 36, 40, 44, 48, 52, 56, 60, 64, 100,\
		104, 108, 112, 116, 120, 124, 128, 132, 136,\
			140, 149, 153, 157, 161, 165 
	};
	unsigned int valid_channels_5_Ghz_40Mhz[]   = { 38, 42, 46, 50, 54, 58, 62, 102,\
		106, 110, 114, 118, 122, 126, 130, 134, 138,\
			151, 155, 159, 163 
	};
	unsigned int valid_channels_4_9_Ghz_20Mhz[]   = { 184, 188, 192, 196, 8, 12, 16, 0xff, 0x00 };
	unsigned int valid_channels_4_9_Ghz_10Mhz[]   = { 183, 184, 185, 187, 188, 189, 7, 8, 9, 11,0xff, 0x00 };
	unsigned char enable_40 = 0;
	unsigned char enable_11j = 0;
	unsigned char rate_flags = 0;
	unsigned char i = 0;
	uint_16 offset = 0;
	uint_16 local_buf[NO_OF_PARAMS];	
	uint_8 chan_number;
	uint_8 send_stats_disable;
	uint_32 stats_cnt;
	uint_32 loop_var = 0;
	uint_32 loop_cnt = 0;
	uint_32 loop_chunk = 10;
	uint_32 write_block = 0;
	int ii=0;
	unsigned short ch_width = 0;

	if (arg == 4 )
	{
		chan_number = packet[0];
		stats_cnt = packet[1];
		send_stats_disable = packet[2]; //do not receive stats
		rate_flags = packet[3];
		ch_width = rate_flags & 0x07; //channel width
		enable_11j = (rate_flags & BIT(3));
    	printf("ch_width: %d \n",ch_width);
	if( ch_width == BW_U40 || ch_width == BW_L40 || ch_width == BW_F40 )
	{
			enable_40 = 1;
	}
		uint_16 *send_rx_packet = malloc(loop_chunk * (NO_OF_PARAMS * SIZE_OF_PARAM));
		if(send_rx_packet == NULL )
		{
			printf("Unable to Allocate Memory\n");
			return ONEBOX_STATUS_FAILURE;
		}	
		per_stats *sta_info = malloc(sizeof(per_stats));
		if (sta_info == NULL)
		{
			printf("Unable to Allocate Memory\n");
			free(send_rx_packet);
			return ONEBOX_STATUS_FAILURE;
		}
		
		memset(local_buf , 0 , NO_OF_PARAMS * SIZE_OF_PARAM);

	if(!enable_11j )
	{
		if (chan_number == 0xFF)
		{
			/* Pass 0xFF so as to skip channel programming */
		}
		else if((chan_number >= 36 && chan_number <= 165 && ch_width == BW_20))
		{
			/* 20Mhz bandwidth, verifying whether the channel is correct */
			for(i = 0; i < 24; i++)
			{
				if(chan_number == valid_channels_5_Ghz[i])
				{
					break;
				}
			}
			if(i == 24)
			{
				printf("Invalid Channel issued by user\n");
				return -1;
			}
		}
    else if((chan_number >= 38 && chan_number <= 163 && enable_40))
    {
			/* 40Mhz bandwidth, verifying whether the channel is correct */
      for(i = 0; i < 21; i++)
      {
        if(chan_number == valid_channels_5_Ghz_40Mhz[i])
        {
          break;
        }
      }
      if(i == 21)
      {
        printf("Invalid Channel issued by user\n");
        return -1;
      }
    }
		else if(!(chan_number <= 14))
		{
			printf("Invalid Channel issued by user\n");
			return -1;
		}
	}
	else
	{
			if(ch_width == BW_20)
			{
					for(i = 0; i < sizeof(valid_channels_4_9_Ghz_20Mhz)/sizeof(valid_channels_4_9_Ghz_20Mhz[0]); i++)
					{
							if(chan_number == valid_channels_4_9_Ghz_20Mhz[i])
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
							if(chan_number == valid_channels_4_9_Ghz_10Mhz[i])
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


		while( stats_cnt )
		{
			loop_cnt = (stats_cnt / loop_chunk);
			if( !loop_cnt )
			{
				write_block = stats_cnt % loop_chunk;
				loop_cnt = 1;
			}
			else
			{
				write_block = loop_chunk;
			}


			for(loop_var = 0; loop_var < loop_cnt; loop_var++)
			{	
				offset = 0;
				memset(send_rx_packet , 0 , loop_chunk * NO_OF_PARAMS*SIZE_OF_PARAM);

				for(ii=0; ii<write_block; ii++)
				{
					memset(sta_info,0,sizeof(per_stats));
					if(sleep(1)!=0)
					{
						printf("Unable to sleep\n");    	
						free(sta_info);	
						break;
					}

					memset(&iwr,0,sizeof(iwr));
					strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
					iwr.u.data.pointer=sta_info;
					iwr.u.data.length = sizeof(per_stats);
					iwr.u.data.flags = (unsigned short)PER_RECEIVE;                                
					iwr.u.data.flags |= (unsigned short)chan_number << 8;
					*(unsigned short *)iwr.u.data.pointer = (unsigned short)rate_flags;

					if(ioctl(sockfd, ONEBOX_HOST_IOCTL,&iwr)<0)
					{
						printf("Unable to issue ioctl\n");
						free(sta_info);
						break;
					}
#if 0
					else
					{
						printf("%7d %7d \n",
								*(unsigned short *)&sta_info[2],
								*(unsigned short *)&sta_info[10]);
					}   
					printf("%12d %12d %12d %12d %12d %12d %12d \n",
							*(unsigned short *)&sta_info[6],
							*(unsigned short *)&sta_info[8],
							*(unsigned short *)&sta_info[10],
							*(unsigned short *)&sta_info[12],
							*(unsigned short *)&sta_info[14],
							*(unsigned short *)&sta_info[16],
							*(unsigned short *)&sta_info[18]);
					*(uint_16*)&packet[0] = htons(*(unsigned short *)&sta_info[2]);
					*(uint_16*)&packet[2] = htons(*(unsigned short *)&sta_info[10]);

#else
					else
					{
						chan_number = 0;
						printf("%2d  %12d %12d %12x \n",
								(ii+1),
								sta_info->crc_pass,
								sta_info->crc_fail,
								sta_info->cal_rssi);

						/* *(unsigned short *)&sta_info[2],
						 *(unsigned short *)&sta_info[10]  );  */
					}   
					//  *(uint_16*)&packet[0] = htons(*(unsigned short *)&sta_info[2]);
					//  *(uint_16*)&packet[2] = htons(*(unsigned short *)&sta_info[10]);


					//	*(uint_16*)&packet[0] = htons(ii+1);   //(*(unsigned short *)&sta_info[2]);
					//	*(uint_16*)&packet[2] = htons(*(unsigned short *)&sta_info[6]);
					//	*(uint_16*)&packet[4] = htons(*(unsigned short *)&sta_info[8]);


					local_buf[0] = htons(ii+1);   //(*(unsigned short *)&sta_info[2]);
					local_buf[1] = htons(sta_info->crc_pass);
					local_buf[2] = htons(sta_info->crc_fail);
					local_buf[3] = htons(sta_info->cal_rssi);
#if 0
					*(uint_16*)&packet[6] = htons(*(unsigned short *)&sta_info[10]);
					*(uint_16*)&packet[8] = htons(*(unsigned short *)&sta_info[12]);
					*(uint_16*)&packet[10] = htons(*(unsigned short *)&sta_info[14]);
					*(uint_16*)&packet[12] = htons(*(unsigned short *)&sta_info[16]);
					*(uint_16*)&packet[14] = htons(*(unsigned short *)&sta_info[18]);

#endif

					memcpy(&send_rx_packet[offset], local_buf, NO_OF_PARAMS * SIZE_OF_PARAM);
					offset = offset + NO_OF_PARAMS;


#endif 
				}
				if (!send_stats_disable)
					onebox_send_packet(send_rx_packet,write_block*(NO_OF_PARAMS * SIZE_OF_PARAM));
				stats_cnt = stats_cnt - write_block;
				printf("no of stats remaining is - %d\n",stats_cnt);
			}
		}

		free(send_rx_packet);

		memset(&iwr,0,sizeof(iwr));
		strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
		iwr.u.data.pointer=sta_info;
		iwr.u.data.length = sizeof(per_stats);
		iwr.u.data.flags = (unsigned short)PER_RECEIVE;                                
		iwr.u.data.flags |= (unsigned short)chan_number << 8;
		iwr.u.data.flags |= (unsigned short)PER_RCV_STOP;
		*(unsigned short *)iwr.u.data.pointer = (unsigned short)rate_flags;

		if(ioctl(sockfd, ONEBOX_HOST_IOCTL,&iwr)<0)
		{
			printf("Unable to issue ioctl\n");
			free(sta_info);
			return ONEBOX_STATUS_FAILURE;	
		}
		free(sta_info);
		
	}
	else
	{
	
		printf("matlab recv stats application\n");   
		printf("Usage: <type> <no_of_arrguments>  <channel num> <no of stats>\n");
		printf("\tno of arguments should always be 3\n");
		printf("\tChannel num 		- Channel to operate\n");
		printf("\tenter the no of stats\n");
		printf("\tenter 1 to disable packets write on to socket and vice-versa\n");

		return 1;
	}

	return ONEBOX_STATUS_SUCCESS;
}


int_32 buffer_read(uint_32 *BUFFER, uint_32 no_of_addr, uint_32 soft_rst)
{
	uint_16 val, i,ii,jj, j= 3,index=0,k=0, buf_len;
	uint_8 blocks, count,count1, write_block = 27;
	struct bb_rf_param_t  bb_rf_params,bb_rf_print;
	uint_32 swap;  
	val = (uint_16)no_of_addr;
	printf("Total no regs are = %d :\n",val);
	blocks = (no_of_addr/(write_block)); 
	count = (no_of_addr%(write_block)); 
	for ( i=0; i<=blocks ; i++)
	{
		printf("");
		if (i == blocks)
		{
			count1 = count;  
		} 
		else 
		{
			count1 = write_block;  
		}      
		index = j;
		printf("index = %d :\n",index);
		bb_rf_params.Data[1] = *(uint_16 *)&BUFFER[0];
		bb_rf_params.Data[2] = *(uint_16 *)&BUFFER[1];
		bb_rf_params.Data[3] = *(uint_16 *)&BUFFER[2];
		for (ii=4,k=1 ; ii<(count1*3 + 3); ii+=3,k++,j+=3)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+1];
			bb_rf_params.Data[ii+2] = ((*(uint_16 *)&BUFFER[j+2]) | 0x80);

			printf("X Data[%d] = %08x ,YData = %08x ,ZData = %08x \n",k,bb_rf_params.Data[ii],bb_rf_params.Data[ii+1],bb_rf_params.Data[ii+2]);
		}
		buf_len = count1;
		printf("Total block = %d :\n",buf_len);
		strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
		bb_rf_params.value = 6;
		bb_rf_params.no_of_values = buf_len;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
		iwr.u.data.pointer = &bb_rf_params;
		iwr.u.data.length = (buf_len); 
		if(ioctl(sockfd,ONEBOX_SET_BB_RF,&iwr)<0)
		{  
			printf("Error reading from buffer\n");
		}  
		else
		{ 
			printf("Buffer reading Successful:\n");
			buf_len = (count1 *3);
			for(ii=0,jj=0;ii<buf_len;ii+=3,jj+=2)
			{
				bb_rf_print.Data[jj] = bb_rf_params.Data[ii+1];
				swap = bb_rf_print.Data[jj];
				bb_rf_print.Data[jj] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
				bb_rf_print.Data[jj+1] = bb_rf_params.Data[ii+2];
				swap = bb_rf_print.Data[jj+1];
				bb_rf_print.Data[jj+1] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
			}
			buf_len = ((buf_len/3)*2)*2;
			printf("Buffer length = %d :\n", buf_len);
			for(ii=0,jj=0;ii<buf_len/2;ii++,jj++)
			{
				printf(" Data 0x%x\n",bb_rf_print.Data[ii]);
			}
			onebox_send_packet((uint_16 *)bb_rf_print.Data, buf_len);

		}
	}  
	printf("Buffer Read Done\n");
	return 0;

}

int_32 rf_write(uint_32 soft_rst, uint_32 num_of_regs, uint_32 vals_per_reg, uint_32 *BUFFER)
{
	uint_16 val, i, j=0 ,index =0, k,ii, rf_len;
	uint_8 blocks, count,count1, write_block = 20;
	struct bb_rf_param_t  bb_rf_params;

	val = (num_of_regs * vals_per_reg);
	printf("Total no regs are = %d :\n",val);
	printf("soft reset = %d, vals_per_reg= %d :\n", soft_rst, vals_per_reg);
	rf_len = val; 
	blocks = (num_of_regs/(write_block)); 
	count = (num_of_regs%(write_block)); 
	for ( i=0; i<=blocks ; i++)
	{
		if (i == blocks)
		{
			count1 = count;  
		} 
		else 
		{
			count1 = write_block;  
		}     
		index = j;
		printf("index = %d :\n",index);
		for (k=1,ii=1 ; ii<count1*5; j+=5,k++,ii+=5)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+3];
			bb_rf_params.Data[ii+2] = *(uint_16 *)&BUFFER[j+2];
			bb_rf_params.Data[ii+3] = *(uint_16 *)&BUFFER[j+1];
			bb_rf_params.Data[ii+4] = *(uint_16 *)&BUFFER[j+4];

			printf("valid_bits= %d, Data2[%d] = %08x ,Data1 = %08x ,Data0 = %08x ,Delay = %d\n",BUFFER[j],
					k,BUFFER[j+1],BUFFER[j+2],BUFFER[j+3],BUFFER[j+4]);
		}
		rf_len = (count1 );
		printf("Total len = %d :\n",rf_len);
		strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
		bb_rf_params.value = 3;
		bb_rf_params.no_of_fields = vals_per_reg;
		bb_rf_params.no_of_values = rf_len;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
		printf("wrinting RF value is:%d\n",bb_rf_params.value);
		printf("wrinting RF no_of_vals is:%d\n",bb_rf_params.no_of_values);
		printf("wrinting RF reset is:%d\n",bb_rf_params.soft_reset);
		printf("wrinting RF fields is:%d\n",bb_rf_params.no_of_fields);
		iwr.u.data.pointer = &bb_rf_params;
		iwr.u.data.length = (rf_len); 
		if(ioctl(sockfd,ONEBOX_SET_BB_RF,&iwr)<0)
		{  
			printf("Error writing from RF\n");
		}  
		else
		{ 
			printf("Writing RF Successful:\n");
		}
	}  
	return 0;
}


int_32 rf_read(uint_32 soft_rst, uint_32 num_of_regs, uint_32 vals_per_reg, uint_32 *BUFFER)
{

	uint_16 val, i, j = 0, index = 0, ii, k, kk = 0,  rf_len;
	uint_8 blocks, count,count1, write_block = 20;
	struct bb_rf_param_t  bb_rf_params, bb_rf_print;
	uint_32 swap;

	val = (num_of_regs * vals_per_reg);
	printf("Total no regs are = %d :\n",val);
	printf("soft reset = %d, vals_per_reg= %d :\n", soft_rst, vals_per_reg);
	rf_len = val; 
	blocks = (num_of_regs/(write_block)); 
	count = (num_of_regs%(write_block)); 
	for ( i=0; i<=blocks ; i++)
	{
		if (i == blocks)
		{
			count1 = count;  
		} 
		else 
		{
			count1 = write_block;  
		}      
		index = j;
		printf("index = %d :\n",index);
		for (ii=1,k=1 ; ii<count1*5; j+=5,k++,ii+=5)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+3];
			bb_rf_params.Data[ii+2] = (*(uint_16 *)&BUFFER[j+2] | BIT(15));
			bb_rf_params.Data[ii+3] = *(uint_16 *)&BUFFER[j+1];
			bb_rf_params.Data[ii+4] = *(uint_16 *)&BUFFER[j+4];

			printf("valid_bits= %d, Data2[%d] = %08x ,Data1 = %08x ,Data0 = %08x ,Delay = %d\n",BUFFER[j],
					k,BUFFER[j+1],BUFFER[j+2],BUFFER[j+3],BUFFER[j+4]);
		}
		rf_len = (count1 );
		printf("Total len = %d :\n",rf_len);
		strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
		bb_rf_params.value = 2;
		bb_rf_params.no_of_fields = vals_per_reg;
		bb_rf_params.no_of_values = rf_len;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
		iwr.u.data.pointer = &bb_rf_params;
		iwr.u.data.length = (rf_len); 
		if(ioctl(sockfd,ONEBOX_SET_BB_RF,&iwr)<0)
		{  
			printf("Error reading from RF\n");
		}  
		else
		{ 
			printf("Reading RF Successful:\n");
			for(ii=0;ii<rf_len;ii+=5)
			{ 
				swap = bb_rf_params.Data[ii];
				bb_rf_print.Data[ii] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
				kk++;
				swap = bb_rf_params.Data[ii+1];
				bb_rf_print.Data[ii+1] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
				kk++;
				printf(" values of data[%d]= %04x data[%d]= %04x\n",ii,bb_rf_params.Data[ii],ii+1,bb_rf_params.Data[ii+1]);
			}
			onebox_send_packet((uint_16 *)bb_rf_print.Data, kk*2);

		}
	}  
	return 0;
}


int_32 ulp_write(uint_32 soft_rst, uint_32 num_of_regs, uint_32 vals_per_reg, uint_32 *BUFFER)
{
	uint_16 val, i, j = 0, index = 0, ii, k, ulp_len;
	uint_8 blocks, count,count1, write_block = 20;
	struct bb_rf_param_t  bb_rf_params;

	val = (num_of_regs * vals_per_reg);
	printf("Total no regs are = %d :\n",val);
	printf("soft reset = %d, vals_per_reg= %d :\n", soft_rst, vals_per_reg);
	ulp_len = val; 
	blocks = (num_of_regs/(write_block)); 
	count = (num_of_regs%(write_block)); 
	for ( i=0; i<=blocks ; i++)
	{
		if (i == blocks)
		{
			count1 = count;  
		} 
		else 
		{
			count1 = write_block;  
		}      
		index = j;
		printf("index = %d :\n",index);
		for (ii=1,k=1 ; ii<count1*5; j+=5,k++,ii+=5)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+1];
			bb_rf_params.Data[ii+2] = *(uint_16 *)&BUFFER[j+2];
			bb_rf_params.Data[ii+3] = *(uint_16 *)&BUFFER[j+3];
			bb_rf_params.Data[ii+4] = *(uint_16 *)&BUFFER[j+4];

			printf("Address[%d]= %08x, %08x ,Data1 = %08x , %08x ,Delay = %d\n",k,BUFFER[j],
					BUFFER[j+1],BUFFER[j+2],BUFFER[j+3],BUFFER[j+4]);
		}
		ulp_len = (count1);
		printf("Total block = %d :\n",ulp_len);
		strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
		bb_rf_params.value = 5;
		bb_rf_params.no_of_values = ulp_len;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
		bb_rf_params.no_of_fields = vals_per_reg;
		iwr.u.data.pointer = &bb_rf_params;
		iwr.u.data.length = (ulp_len); 
		if(ioctl(sockfd,ONEBOX_SET_BB_RF,&iwr)<0)
		{  
			printf("Error writing from ULP\n");
		}  
		else
		{ 
			printf("Writing ULP Successful:\n");
		}
	}  
	return 0;
}


int_32 ulp_read(uint_32 soft_rst, uint_32 num_of_regs, uint_32 vals_per_reg, uint_32 *BUFFER)
{

	uint_16 val, i, j = 0, index = 0, ii , k, m, ulp_len, swap;
	uint_8 blocks, count,count1, write_block = 20;
	struct bb_rf_param_t  bb_rf_params;

	val = (num_of_regs * vals_per_reg);
	printf("Total no regs are = %d :\n",val);
	printf("soft reset = %d, vals_per_reg= %d :\n", soft_rst, vals_per_reg);
	ulp_len = val; 
	blocks = (num_of_regs/(write_block)); 
	count = (num_of_regs%(write_block)); 
	for ( i=0; i<=blocks ; i++)
	{
		if (i == blocks)
		{
			count1 = count;  
		} 
		else 
		{
			count1 = write_block;  
		}      
		index = j;
		printf("index = %d :\n",index);
		for (ii=1,k=1 ; ii<count1*5; j+=5,k++,ii+=5)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+1];
			bb_rf_params.Data[ii+2] = *(uint_16 *)&BUFFER[j+2];
			bb_rf_params.Data[ii+3] = *(uint_16 *)&BUFFER[j+3];
			bb_rf_params.Data[ii+4] = *(uint_16 *)&BUFFER[j+4];

			printf("Address[%d]= %08x, %08x ,Data1 = %08x , %08x ,Delay = %d\n",k,BUFFER[j],
					BUFFER[j+1],BUFFER[j+2],BUFFER[j+3],BUFFER[j+4]);
		}
		ulp_len = (count1 );
		printf("Total block = %d :\n",ulp_len);
		strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
		bb_rf_params.value = 4;
		bb_rf_params.no_of_values = ulp_len;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
		bb_rf_params.no_of_fields = vals_per_reg;
		iwr.u.data.pointer = &bb_rf_params;
		iwr.u.data.length = (ulp_len); 
		if(ioctl(sockfd,ONEBOX_SET_BB_RF,&iwr)<0)
		{  
			printf("Error reading from ULP\n");
		}  
		else
		{ 
			printf("Reading ULP Successful:\n");
			for(m=0,k=0;m<ulp_len;m+=5,k+=2)
			{
				printf(" values of data[%d]= %x\n",m,bb_rf_params.Data[m]);
				bb_rf_params.Data[k+1] = (bb_rf_params.Data[m+2]);
				swap = bb_rf_params.Data[k+1];
				bb_rf_params.Data[k+1] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
				bb_rf_params.Data[k] =(bb_rf_params.Data[m+3]);
				swap = bb_rf_params.Data[k];
				bb_rf_params.Data[k] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
			}
			printf(" length is [%d]= \n",k);
			for(m=0;m<k;m++)
			{
				if (!bb_rf_params.Data[m])
					bb_rf_params.Data[m] = 0x0000;
				printf(" sending data[%d]= %x\n",m,bb_rf_params.Data[m]);

			}
			onebox_send_packet((uint_16 *)bb_rf_params.Data, (k*2));

		}
	}  
	return 0;
}

int_32 bb_write(uint_32 bb_len, uint_32 soft_rst, uint_32 * BUFFER)
{
	uint_16 val, i,j = 0, index = 0, ii, k;
	uint_8 blocks, count,count1, write_block = 50;
	val = (uint_16)bb_len;
	struct bb_rf_param_t  bb_rf_params;
	per_params_t  per_params;

	memset(&bb_rf_params,0,sizeof(bb_rf_params));
	memset(&iwr,0,sizeof(iwr));
	blocks = (bb_len/(write_block)); 
	count = (bb_len%(write_block)); 
	for ( i=0; i<=blocks ; i++)
	{
		if (i == blocks)
		{
			count1 = count;  
		} 
		else 
		{
			count1 = write_block;  
		}      
		index = j;
		printf("index = %d :\n",index);
		for (ii=1,k=1 ; ii<count1*2; j+=2,ii+=2,k++)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+1];

			printf(" addr[%d]= %08x, Data= %08x\n",k,bb_rf_params.Data[j],bb_rf_params.Data[j+1]);
		}

		val = (count1 * 2);
		printf("writing BB len is:%d\n",val);
		strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
		bb_rf_params.value = 1;
		bb_rf_params.no_of_values = val;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
		printf("writing BB value is:%d\n",bb_rf_params.value);
		printf("writing BB no_of_vals is:%d\n",bb_rf_params.no_of_values);
		printf("writing BB reset is:%d\n",bb_rf_params.soft_reset);
		iwr.u.data.pointer = &bb_rf_params;
		iwr.u.data.length = 1; 
		if (ioctl(sockfd, ONEBOX_SET_BB_RF, &iwr) < 0) 
			printf("Error writing to BB\n");
		else
			printf("SUCCESS Writing to BB\n");

	}  
	return 0;
}


int_32 lmac_rf_lpbk_m2(uint_32 bb_len, uint_32 soft_rst)
{

	int_32 count,cc=1,m = 0;
	uint_32 addr,ref,swap,i;
	//uint_16 *temp;
	//uint_16  *bb_rf_params;
	struct bb_rf_param_t  *bb_rf_params, bb_print_params;
	count = bb_len;
	printf("Num of addrs are 0x%x, \n",bb_len);
	strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
	unsigned char *temp = malloc(count*2);
	memset(temp,0,count*2);
	bb_rf_params = (struct bb_rf_param_t *)temp;
	bb_rf_params->no_of_values = (uint_8)bb_len*2;
	bb_rf_params->value = LMAC_RF_M2;
	bb_rf_params->soft_reset = (uint_8)soft_rst;

	iwr.u.data.pointer = bb_rf_params;
	iwr.u.data.length  = (bb_len);
	iwr.u.data.length  = 1;
	iwr.u.data.length = bb_len = 4096/2;
	printf(" length is %d \n",iwr.u.data.length);
	printf(" bb_rf is %p, wrq: %p, temp : %p \n",bb_rf_params,iwr.u.data.pointer,temp);
	if(ioctl(sockfd,ONEBOX_SET_BB_RF,&iwr)<0)
		printf("Error Reading in LMAC_RF_READ \n");
	else
	{
		printf("\n LMAC_RF_LPBK_READ SUCCESS\n");
		printf("Address           Data\n");

		for(m=0;m<bb_len*2;m+=2)
		{
			printf("After swap 0x%x            0x%x\n",temp[m],temp[m+1]);
		}
	}
	onebox_send_packet((uint_16 *)temp, (bb_len)*2);
	// sleep(10); 
	//	onebox_send_packet((uint_16 *)(temp + bb_len/2) , (bb_len));
	return 0;
}

int_32 lmac_rf_lpbk_m3(uint_32 bb_len, uint_32 soft_rst)
{

	int_32 count,cc=1,m = 0;
	uint_32 addr,ref,swap,i;
  //uint_16 *temp;
	//uint_16  *bb_rf_params;
	struct bb_rf_param_t  *bb_rf_params, bb_print_params;
	count = bb_len;
	printf("Num of addrs are 0x%x, \n",bb_len);
		strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
	  unsigned char *temp = malloc(count*2);
		memset(temp,0,count*2);
	 bb_rf_params = (struct bb_rf_param_t *)temp;
	bb_rf_params->no_of_values = (uint_8)bb_len*2;
	bb_rf_params->value = LMAC_RF_M3;
	bb_rf_params->soft_reset = (uint_8)soft_rst;

	iwr.u.data.pointer = bb_rf_params;
	iwr.u.data.length  = (bb_len);
	iwr.u.data.length  = 1;
  iwr.u.data.length = bb_len = 512/2;
	printf(" length is %d \n",iwr.u.data.length);
	printf(" bb_rf is %p, wrq: %p, temp : %p \n",bb_rf_params,iwr.u.data.pointer,temp);
	if(ioctl(sockfd,ONEBOX_SET_BB_RF,&iwr)<0)
		printf("Error Reading in LMAC_RF_READ \n");
	else
	{
		printf("\n LMAC_RF_LPBK_M3 SUCCESS\n");
		printf("Address           Data\n");

    for(m=0;m<bb_len*2;m+=2)
		{
//			printf(" 0x%x     0x%x\n",temp[m],(uint_8) temp[m+1]);
//			swap = temp[m];
//			temp[m] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
			printf("After swap 0x%x            0x%x\n",temp[m],temp[m+1]);
		}
	}
#if 0 
	for(i=0;i<bb_len;i++)
	{
		swap = bb_rf_params.Data[i];
		bb_rf_params.Data[i] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
	}
#endif  
	onebox_send_packet((uint_16 *)temp, (bb_len)*2);
 // sleep(10); 
 //	onebox_send_packet((uint_16 *)(temp + bb_len/2) , (bb_len));
	return 0;
}

int_32 bb_read(uint_32 bb_addr, uint_32 bb_len, uint_32 soft_rst)
{

	int_32 count,cc=1,m;
	uint_32 addr,ref,swap,i;
	struct bb_rf_param_t  bb_rf_params, bb_print_params;
	count = bb_len;
	addr = bb_addr;
	printf(" Final read format for base band is 0x%x length is 0x%x :\n",addr,count);
	while(count)
	{
		strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
		ref = addr;
		bb_print_params.Data[cc-1] = addr;
		printf(" address value is %x :\n",addr);
		addr = addr + 1;


		count--;
		bb_rf_params.Data[cc] = (uint_16)ref;
		cc++;
	}
	printf("Num of addrs are 0x%x, bb-rfparms.Data = 0x%x\n",bb_len,bb_rf_params.Data[0]);
	bb_rf_params.no_of_values = (uint_8)bb_len;
	bb_rf_params.value = 0x0000;
	bb_rf_params.soft_reset = (uint_8)soft_rst;

	iwr.u.data.pointer = &bb_rf_params;
	iwr.u.data.length  = (bb_len);
	printf(" length is %d \n",iwr.u.data.length);
	if(ioctl(sockfd,ONEBOX_SET_BB_RF,&iwr)<0)
		printf("Error Reading in BB_READ \n");
	else
	{
		printf("\n BB_READ success\n");
		printf("Address           Data\n");
		for(m=0;m<bb_len;m++)
		{
			printf(" 0x%x            0x%x\n",bb_print_params.Data[m],bb_rf_params.Data[m]);
			swap = bb_rf_params.Data[m];
			bb_rf_params.Data[m] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
			printf("After swap 0x%x            0x%x\n",bb_print_params.Data[m],bb_rf_params.Data[m]);
		}
	}
#if 0 
	for(i=0;i<bb_len;i++)
	{
		swap = bb_rf_params.Data[i];
		bb_rf_params.Data[i] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
	}
#endif  
	onebox_send_packet((uint_16 *)bb_rf_params.Data, (bb_len*2));
	return 0;
}

int_32 rf_reset(uint_32 type, uint_32 delay )
{

	struct bb_rf_param_t  bb_rf_params ;
	printf(" RF_resetting type 0x%x  delay is 0x%x :\n",type,delay);
	strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
	bb_rf_params.Data[1] = (uint_16)type;
	bb_rf_params.Data[2] = (uint_16)delay;
	bb_rf_params.no_of_values = (uint_8)2;
	bb_rf_params.value = RF_RESET;

	iwr.u.data.pointer = &bb_rf_params;
	iwr.u.data.length  = (2);
	if(ioctl(sockfd,ONEBOX_SET_BB_RF,&iwr)<0)
		printf("Error Reading in RF_RESET \n");
	else
	{
		printf("\n BB_READ success\n");
	}  
	return 0;
}

int_32 set_endpoint(uint_16 type)
{

	struct bb_rf_param_t  bb_rf_params ;
	printf(" Endpoint type 0x%x  :\n",type);
	strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);

	iwr.u.data.length  = (2);
	iwr.u.data.flags = type << 8; //endpoint type
	iwr.u.data.flags |= SET_ENDPOINT; 
	if(ioctl(sockfd, ONEBOX_HOST_IOCTL, &iwr) < 0) 
	{
		ONEBOX_PRINT("Error while setting endpoint\n");
	}
	else
	{
		printf("\n ENDPOINT success\n");
	}  
	return 0;
}

int_32 soc_reg_access(uint_32 type, uint_32 addr, uint_32 data )
{

	struct bb_rf_param_t  bb_rf_params ;
	uint_16 swap = 0;
	printf(" LMAC_REG  type: 0x%x addr 0x%x  data is 0x%x :\n",type, addr, data);
	strncpy(iwr.ifr_name,"rpine0",IFNAMSIZ);
	bb_rf_params.Data[1] = (uint_16)(addr);
	bb_rf_params.Data[2] = (uint_16)(addr >> 16);
	bb_rf_params.Data[3] = (uint_16)(data);
	bb_rf_params.Data[4] = (uint_16)(data >> 16);
	printf("  Addr1 0x%x  Addr2 0x%x\n",bb_rf_params.Data[1],bb_rf_params.Data[2]);
	printf("  Data 0x%x  Data 0x%x\n",bb_rf_params.Data[3],bb_rf_params.Data[4]);
	bb_rf_params.no_of_values = (uint_8)4;
	bb_rf_params.value = type;
	// type 0 - read and 1 - write
	iwr.u.data.pointer = &bb_rf_params;
	iwr.u.data.length  = (2);
	if(ioctl(sockfd,ONEBOX_SET_BB_RF,&iwr)<0)
		printf("Error Reading in LMAC_REG_READ/WRITE \n");
	else
	{
		printf("\n LMAC_REG_READ/WRITE success\n");
		if (type == SOC_REG_READ)
		{
			{
				printf(" 0x%x            0x%x\n",bb_rf_params.Data[0],bb_rf_params.Data[1]);
				swap = bb_rf_params.Data[0];
				bb_rf_params.Data[0] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
				//        printf("After swap 0x%x            0x%x\n",bb_print_params.Data[m],bb_rf_params.Data[m]);
			}
			onebox_send_packet((uint_16 *)bb_rf_params.Data, (2));
		}
	}  
	return 0;
}

int udp_server(void)
{
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	uint_8 packet[ONEBOX_MAX_PKT_LEN];
	uint_32 read_len, client_len;
	ONEBOX_STATUS status;

	FUNCTION_ENTRY();

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SOURCE_PORT_NUM);

	conn_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (bind(conn_socket, (struct sockaddr*)&server_addr,
				sizeof(server_addr)) == -1) {
		printf ("udp_server: Binding Failed\n");
	}

	/* Run the UDP server */ 
	printf("**********************************************\n");
	printf("***** Server Is Listening On UDP Port %d *****\n", SOURCE_PORT_NUM);
	printf("***********************************************\n");

	while ( 1 ) {
		client_len = sizeof(client_addr);
		read_len = recvfrom( conn_socket, packet, ONEBOX_MAX_PKT_LEN, 
				0, (struct sockaddr*)&client_addr, &client_len);

		printf("Received PKT Length from MATLAB : %d(0x%x)\n", read_len, read_len);
		printf ("Received Packet in UDP :");
		onebox_dump(packet, read_len);

		/* Process the packet */
		status = onebox_process_packet(packet, read_len, &client_addr, client_len);
		if (status == ONEBOX_STATUS_FAILURE) {
			printf("Closing the Server\n");
			break;
		}
	}
	close(conn_socket);

	FUNCTION_EXIT();
	return 0;
}

int tcp_server(void)
{
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	uint_8 packet[ONEBOX_MAX_PKT_LEN];
	uint_8 packet1[ONEBOX_MAX_PKT_LEN];
	uint_32 read_len, client_len, i = 0, burst = 0;
	ONEBOX_STATUS status;

	FUNCTION_ENTRY();

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(SOURCE_PORT_NUM);

	conn_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(conn_socket, (struct sockaddr*)&server_addr,
				sizeof(server_addr)) == -1) {
		printf ("tcp_server: Binding Failed\n");
	}

	if (listen(conn_socket, 5) == -1) {
		perror("Listen");
		exit(1);
	}

	/* Run the TCP server */
	printf("**************************************************\n");
	printf("***** Server Is Listening On TCP Port %d *****\n", SOURCE_PORT_NUM);
	printf("**************************************************\n");

	client_sock = accept(conn_socket, 
			(struct sockaddr*)&client_addr, &client_len);
	while (1) {
		client_len = sizeof(client_addr);

		read_len = recv(client_sock, packet, 5000, 0);
		printf("Received PKT Length from MATLAB : %d(0x%x)\n", read_len, read_len);
		printf ("Received Packet :");
		//onebox_dump(packet, read_len);
		i = read_len;
		burst = ntohl(*(&packet[0]));
		burst = ntohl(*(uint_32*)&packet[0]);
		printf ("Received burst :%d 0x%8x",burst, packet[0]);

		if (packet1[0] ==6 || packet1[0] ==7 || burst == 6 || burst == 7)
		{
			if (read_len < 3000)
			{
				printf("read_len1 : %d  i : (%d)\n", read_len, i);
				read_len = recv(client_sock, &packet[i++], 5000, 0);
				i+= read_len;
				read_len = i;
				printf("read_len_final : %d  i : (%d)\n", read_len, i);
				//                  if (burst == 2 | burst == 2)
				//                    for (i=0 ;i<read_len;i++)
				//                      //                      packet1[i] = packet[i];
			}      
		}  
		/* Process the received Packet */
		status = onebox_process_packet(packet, read_len, 
				&client_addr, client_len);
		if (status == ONEBOX_STATUS_FAILURE) {
			printf("Closing the Server\n");
			break;
		}
	}

	close(client_sock);
	close(conn_socket);

	FUNCTION_EXIT();
	return 0;
}
int main(int argc, char **argv)
{
	FUNCTION_ENTRY();
	printf ("Creating interface to rpine0\n");

	/* Open the Interface */    
	if (onebox_open_interface()) {
		printf("Failed creating interface!!!\n");
		return -1;
	} else {
		printf("Successfully Created interface to rpine0\n");
	}

#ifdef UDP_CONN
	/*** Starting UDP Server ***/
	udp_server();
#else
	/*** Starting TCP Server ***/
	tcp_server();
#endif
	close(sockfd);
	printf ("Device Closed\n");
	FUNCTION_EXIT();
	return 0;
}


