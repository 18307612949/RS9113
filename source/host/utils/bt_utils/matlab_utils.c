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
#include "bt_util.h"

int sockfd;
int conn_socket;
int client_sock;

int_32 rf_read(uint_32, uint_32, uint_32, uint_32 *);
int_32 rf_write(uint_32, uint_32, uint_32, uint_32 *);
int_32 bb_read(uint_32, uint_32, uint_32);
int_32 bb_write(uint_32, uint_32, uint_32 *);
int_32 buffer_read(uint_32 *, uint_32, uint_32);
int_32 buffer_write(uint_32 *, uint_32, uint_32);
int_32 rf_reset(uint_32 ,uint_32);
int_32 bt_transmit(uint_32 *,uint_32);
int_32 bt_receive(uint_32 *);
int_32 bt_ber_receive(uint_32 );

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
  uint_32 no_of_args;
  uint_32 no_of_packets = 0;;
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

			for(ii=12, jj=0; ii<rf_len*4+12; ii+=4, jj++)
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

			for(ii=12, jj=0; ii<rf_len*4+12; ii+=4, jj++)
			{
				BUFFER[jj] = ntohl(*(uint_32*)&packet[ii+4]);
			}
#if 0
			for(jj=0, i=0; jj<rf_len; jj+=5, i++)
				printf("valid_bits= %d, Data2[%d] = %08x ,Data1 = %08x ,Data0 = %08x ,Delay = %d\n",BUFFER[jj],
						i,BUFFER[jj+1],BUFFER[jj+2],BUFFER[jj+3],BUFFER[jj+4]);
#endif
			rf_write(soft_reset, num_of_regs, vals_per_reg, BUFFER);
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
			for(ii=20, jj=3; ii<=no_of_addr*12+20; ii=ii+4, jj++)
			{
				BUFFER[jj] = ntohl(*(uint_32*)&packet[ii+4]);
			}

#if 1
			for(jj=3; jj<no_of_addr*3+3; jj+=3)
				printf(" XData2[%d] = %08x ,YData= %08x ,ZData= %08x \n", (jj-3), BUFFER[jj], BUFFER[jj+1], BUFFER[jj+2]);
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
			printf(" X_reg_addr= %08x, Y_reg_addr= %08x, Z_reg_addr= %08x \n", BUFFER[0], BUFFER[1], BUFFER[2]);
			for(ii=20, jj=3; ii<=no_of_addr*12+20; ii=ii+4, jj++)
			{
				BUFFER[jj] = ntohl(*(uint_32*)&packet[ii+4]);
			}

#if 1
			for(jj=3; jj<no_of_addr*3+3; jj+=3)
				printf(" XData2[%d] = %08x ,YData= %08x ,ZData= %08x ,Delay = %d\n",(jj-3),BUFFER[jj],BUFFER[jj+1],BUFFER[jj+2],BUFFER[jj+3]);
#endif

			buffer_write(BUFFER, no_of_addr, soft_reset);
			break;
    case BT_PER_TRANSMIT:
      no_of_args = ntohl(*(uint_32*)&packet[4]);
			for( i=0, j=0 ; i < no_of_args; i++ )
			{
				BUFFER[i] = ntohl(*(uint_32 *)&packet[j + 8]);
				j = j+4;
			}
      bt_transmit(BUFFER, no_of_args);
      break;
    case BT_RECEIVE:
      no_of_args = ntohl(*(uint_32*)&packet[4]);
			for( i=0, j=0 ; i < no_of_args; i++ )
			{
				BUFFER[i] = ntohl(*(uint_32 *)&packet[j + 8]);
				j = j+4;
			}
      if( no_of_args != 14 ) 
      {
         printf("Usage:<no_of_args(14)> <BD_ADDR[6:4]> <BD_ADDR[4:0]> <LINK_TYPE> <PKT_TYPE> <PKT_LENGTH> <SCRAMBLER_SEED> <EDR_INDICATION> <RX_CHNL> <TX_CHNL> <MODE> <LE_CHANNEL> <enable_hoping> <ant_sel>  <no_of_stats>\n");
      }
      bt_receive(BUFFER);
      break;
    case BT_BER_RECEIVE:
      no_of_args = ntohl(*(uint_32*)&packet[4]);
      if( no_of_args != 1 )
      {
        printf("Usage: <no_of_arg(1)>  < no_of_packet to send>");
        break;
      }
      no_of_packets = ntohl(*(uint_32*)&packet[8]);
      bt_ber_receive(no_of_packets);
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
		bb_rf_params.Data[0] = *(uint_16 *)&BUFFER[0];
		bb_rf_params.Data[1] = *(uint_16 *)&BUFFER[1];
		bb_rf_params.Data[2] = *(uint_16 *)&BUFFER[2];
		for (ii=3, k=1 ; ii<(count1*3 + 3); ii+=3, k++, j+=3)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+1];
			bb_rf_params.Data[ii+2] = ((*(uint_16 *)&BUFFER[j+2]) | 0x40) ;

			printf("X Data[%d] = %08x ,YData = %08x ,ZData = %08x \n",k, bb_rf_params.Data[ii], bb_rf_params.Data[ii+1], bb_rf_params.Data[ii+2]);
		}
		buf_len = count1;
		printf("Total block = %d :\n", buf_len);
		bb_rf_params.value = 7;
		bb_rf_params.no_of_values = buf_len;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
    if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER, sizeof(bb_rf_params)) < 0 )
    {
      printf("Unable to perform BUFFER WRITE\n");
      return -1;
    }
		else
		{ 
     // if( recv_data(&bb_rf_params.Data[0]) < 0 )
     // {
     //   return -1;
     // }
			printf("Buffer Writing Successful:\n");
		}
	}  
	return 0;

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
		bb_rf_params.Data[0] = *(uint_16 *)&BUFFER[0];
		bb_rf_params.Data[1] = *(uint_16 *)&BUFFER[1];
		bb_rf_params.Data[2] = *(uint_16 *)&BUFFER[2];
		for (ii=3,k=1 ; ii<(count1*3 + 3); ii+=3,k++,j+=3)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+1];
			bb_rf_params.Data[ii+2] = ((*(uint_16 *)&BUFFER[j+2]) | 0x80);

			printf("X Data[%d] = %08x ,YData = %08x ,ZData = %08x \n",k, bb_rf_params.Data[ii], bb_rf_params.Data[ii+1], bb_rf_params.Data[ii+2]);
		}
		buf_len = count1;
		printf("Total block = %d :\n",buf_len);
		bb_rf_params.value = 6;
		bb_rf_params.no_of_values = buf_len;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
    if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER, sizeof(bb_rf_params)) < 0 )
    {
      printf("Unable to perform buffer read\n");
      return -1;
    }
		else
		{ 
      memset(&bb_rf_params,0,sizeof(bb_rf_params));
      if( recv_data( (uint_8*)&bb_rf_params.Data[0]) < 0 )
      {
        return -1;
      }
			printf("Buffer reading Successful:\n");
			buf_len = (count1 *3);
			for(ii=0, jj=0; ii<buf_len; ii+=3, jj+=2)
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
			for(ii=0, jj=0; ii<buf_len/2; ii++, jj++)
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
		for (k=1,ii=0 ; ii<count1*5; j+=5, k++, ii+=5)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j+3];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+2];
			bb_rf_params.Data[ii+2] = *(uint_16 *)&BUFFER[j+1];
			bb_rf_params.Data[ii+3] = *(uint_16 *)&BUFFER[j+4];
			bb_rf_params.Data[ii+4] = *(uint_16 *)&BUFFER[j];

			printf("valid_bits= %d, Data2[%d] = %08x ,Data1 = %08x ,Data0 = %08x ,Delay = %d\n", BUFFER[j],
					k, BUFFER[j+1], BUFFER[j+2], BUFFER[j+3], BUFFER[j+4]);
		}
		rf_len = (count1 );
		printf("Total len = %d :\n",rf_len);
		bb_rf_params.value = 3;
		bb_rf_params.no_of_fields = vals_per_reg = 3;
		bb_rf_params.no_of_values = rf_len;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
		printf("wrinting RF value is:%d\n", bb_rf_params.value);
		printf("wrinting RF no_of_vals is:%d\n", bb_rf_params.no_of_values);
		printf("wrinting RF reset is:%d\n", bb_rf_params.soft_reset);
		printf("wrinting RF fields is:%d\n", bb_rf_params.no_of_fields);
    if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER, sizeof(bb_rf_params)) < 0 )
    {
      printf("Unable to perform RF_WRITE\n");
      break;
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
		for (ii=1,k=1 ; ii<count1*5; j+=5, k++, ii+=5)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+3];
			bb_rf_params.Data[ii+2] = (*(uint_16 *)&BUFFER[j+2] | BIT(15));
			bb_rf_params.Data[ii+3] = *(uint_16 *)&BUFFER[j+1];
			bb_rf_params.Data[ii+4] = *(uint_16 *)&BUFFER[j+4];

			printf("valid_bits= %d, Data2[%d] = %08x ,Data1 = %08x ,Data0 = %08x ,Delay = %d\n",BUFFER[j],
					k, BUFFER[j+1], BUFFER[j+2], BUFFER[j+3], BUFFER[j+4]);
		}
		rf_len = (count1 );
		printf("Total len = %d :\n",rf_len);
		bb_rf_params.value = 2;
		bb_rf_params.no_of_fields = vals_per_reg;
		bb_rf_params.no_of_values = rf_len;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
    if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER, sizeof(bb_rf_params)) < 0 )
    {
      printf("Unable to perform RF_READ\n");
      break;
    }
		else
		{ 
      if( recv_data((uint_8 *)&bb_rf_params.Data[0]) < 0)
      {
        return -1;
      }
			printf("Reading RF Successful:\n");
			for(ii=0; ii<rf_len; ii+=5)
			{ 
				swap = bb_rf_params.Data[ii];
				bb_rf_print.Data[ii] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
				kk++;
				swap = bb_rf_params.Data[ii+1];
				bb_rf_print.Data[ii+1] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
				kk++;
				printf(" values of data[%d]= %04x data[%d]= %04x\n",ii, bb_rf_params.Data[ii], ii+1, bb_rf_params.Data[ii+1]);
			}
			onebox_send_packet((uint_16 *)bb_rf_print.Data, kk*2);
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

	memset(&bb_rf_params,0,sizeof(bb_rf_params));
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
		for (ii=0,k=1 ; ii<count1*2; j+=2,ii+=2,k++)
		{
			bb_rf_params.Data[ii] = *(uint_16 *)&BUFFER[j];
			bb_rf_params.Data[ii+1] = *(uint_16 *)&BUFFER[j+1];

			printf(" addr[%d]= %08x, Data= %08x\n",k, bb_rf_params.Data[j], bb_rf_params.Data[j+1]);
		}

		val = (count1 * 2);
		printf("writing BB len is:%d\n",val);
		bb_rf_params.value = 1;
		bb_rf_params.no_of_values = val;
		bb_rf_params.soft_reset = (uint_8)soft_rst;
		printf("writing BB value is:%d\n",bb_rf_params.value);
		printf("writing BB no_of_vals is:%d\n",bb_rf_params.no_of_values);
    printf("writing BB reset is:%d\n",bb_rf_params.soft_reset);
    if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER,sizeof(bb_rf_params)) < 0 )
    {
      printf("Unable to perform BB_WRITE\n");
      return -1;
    }
    else
      printf("SUCCESS Writing to BB\n");

	}  
	return 0;
}
int_32 bb_read(uint_32 bb_addr, uint_32 bb_len, uint_32 soft_rst)
{

	int_32 count,cc=0,m;
	uint_32 addr,ref,swap,i;
	struct bb_rf_param_t  bb_rf_params, bb_print_params;
	count = bb_len;
	addr = bb_addr;
	printf(" Final read format for base band is 0x%x length is 0x%x :\n", addr, count);
  memset(&bb_rf_params, 0, sizeof(bb_rf_params));
	while(count)
	{
		ref = addr;
		bb_print_params.Data[cc] = addr;
		printf(" address value is %x :\n",addr);
		addr = addr + 1;
		count--;
		bb_rf_params.Data[cc] = (uint_16)ref;
		cc++;
	}
	printf("Num of addrs are 0x%x, bb-rfparms.Data = 0x%x\n", bb_len, bb_rf_params.Data[0]);
	bb_rf_params.no_of_values = (uint_8)bb_len;
	bb_rf_params.value = 0x0000;
	bb_rf_params.soft_reset = (uint_8)soft_rst;

  if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER, sizeof(bb_rf_params)) < 0 )
  {
    printf("Unable to perform BB_READ\n");
    return -1;
  }
	else
	{
    memset(&bb_rf_params, 0, sizeof(bb_rf_params));
    if( recv_data((uint_8 *)&bb_rf_params.Data[0]) < 0 )
    {
      return -1;
    }
		printf("\n BB_READ success\n");
		printf("Address           Data\n");
		for(m=0;m<bb_len;m++)
		{
			printf(" 0x%x            0x%x\n", bb_print_params.Data[m], bb_rf_params.Data[m]);
			swap = bb_rf_params.Data[m];
			bb_rf_params.Data[m] = ((swap & 0x00FF) << 8) | ((swap & 0xFF00) >> 8);
			printf("After swap 0x%x            0x%x\n", bb_print_params.Data[m], bb_rf_params.Data[m]);
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
int_32 bt_transmit(uint_32 *packet, uint_32 no_of_args)
{
	int i;
	char *tmp_rate;
	bt_per_params_t bt_per_params;
  struct bb_rf_param_t bb_rf_params;
  memset(&bb_rf_params,0,sizeof(bb_rf_params));
  if( no_of_args == 17 )
  {
    *(unsigned short int *)&bt_per_params.bt_addr[4] = packet[0];
    *(unsigned int *)&bt_per_params.bt_addr[0] = packet[1];
    bt_per_params.pkt_type = packet[2];
    bt_per_params.pkt_length = packet[3];
    bt_per_params.edr_indication = packet[4];
    bt_per_params.bt_rx_rf_chnl = packet[5];
    bt_per_params.bt_tx_rf_chnl = packet[6];
    bt_per_params.link_type = packet[7];
    bt_per_params.scrambler_seed = packet[8];
    bt_per_params.num_pkts = packet[9];
    bt_per_params.payload_data_type = packet[10];
    bt_per_params.mode = packet[11];
    bt_per_params.le_chnl = packet[12];
    bt_per_params.tx_pwr_indx = packet[13];
    bt_per_params.transmit_mode = packet[14];
    bt_per_params.enable_hopping = packet[15];
    bt_per_params.ant_select = packet[16];

    printf("the packet type is %d \n",bt_per_params.pkt_type);
    printf("the packet length is %d \n",bt_per_params.pkt_length);

    bb_rf_params.value = BT_PER_TRANSMIT;
    bb_rf_params.no_of_values = sizeof(bt_per_params_t);
    memcpy(&bb_rf_params.Data[0], &bt_per_params, sizeof(bt_per_params_t));

    if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER, sizeof(bb_rf_params)) < 0 )
    {
      printf("Unable to perform bt_transmit\n");
      return -1;
    }
    printf("======== SUCCESS ============\n");
  }
  else if( no_of_args == 1)
  {
    bt_per_params.enable = 0;
    printf("============= BT_TRANSMIT_STOP ==================");

    bb_rf_params.value = BT_PER_TRANSMIT;
    bb_rf_params.no_of_values = sizeof(bt_per_params_t);
    memcpy(&bb_rf_params.Data[0], &bt_per_params, sizeof(bt_per_params_t));

    if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER, sizeof(bb_rf_params)) < 0 )
    {
      printf("Unable to perform bt_transmit\n");
      return -1;
    }
    printf("======== SUCCESS ============\n");
  }
  else
  {
		printf("\nUSAGE to start transmit: < no_of_args(17) > <BT_ADDR[47:32]> <BT_ADDR[31:0]> <PKT_TYPE[0:16]> \n <LENGTH> <EDR_INDICATION> <BT_RX_RF_CHNL> <BT_TX_RF_CHNL> <LINK_TYPE[ACL-1/SCO-2/ESCO-3]>\n <SCRAMBLER_SEED> <NO_OF_PACKETS> <PAYLOAD_TYPE> <MODE(BREDR = 1/LE = 2)> <LE_CHANNEL(ADV = 0, DATA = 1)> <TX_POWER>\n <TRANSMIT_MODE(BURST = 0, CONTI = 1)> <HOPPING_TYPE(NO HOPPING = 0, FIXED HOPPING = 1, RANDOM HOPPING = 2)> <ANTENNA SELECT>\n");
		printf("\nUSAGE to stop  transmit: < no_of_args(1) > < 0 >\n");
  }
}
#define NO_OF_PARAMS 4
#define SIZE_OF_PARAM 2
bt_receive(uint_32 *packet)
{
  bt_per_recv_params_t bt_per_recv_params;
  struct bb_rf_param_t bb_rf_params;
  int stop_bit = 0;
  int i; 
  struct bt_per_stats_s *sta_info;
  uint_16 offset = 0;
  uint_8 no_of_stats = 0;
  uint_32 loop_var = 0;
  uint_32 loop_cnt = 0;
  uint_32 loop_chunk = 10;
  uint_32 write_block = 0;
  uint_16 count = 0;
  int first_time = 1;
  int ii=0;


  memset(&bt_per_recv_params, 0, sizeof(bt_per_recv_params_t));
  uint_16 local_buf[NO_OF_PARAMS];	
  uint_16 *send_rx_packet = malloc(loop_chunk * (NO_OF_PARAMS * SIZE_OF_PARAM));

  memset(local_buf , 0 , NO_OF_PARAMS * SIZE_OF_PARAM);

  bt_per_recv_params.enable = 1;
  *(unsigned int *)&bt_per_recv_params.bt_addr[0] = packet[1];
  *(unsigned short int *)&bt_per_recv_params.bt_addr[4] = packet[0];
  bt_per_recv_params.link_type = packet[2];
  bt_per_recv_params.pkt_type = packet[3];
  bt_per_recv_params.pkt_length = packet[4];
  bt_per_recv_params.scrambler_seed = packet[5];
  bt_per_recv_params.edr_indication = packet[6];
  bt_per_recv_params.bt_rx_rf_chnl = packet[7];
  bt_per_recv_params.bt_tx_rf_chnl = packet[8];
  bt_per_recv_params.le_mode = packet[9];
  bt_per_recv_params.le_chnl = packet[10];
  bt_per_recv_params.enable_hopping = packet[11];
  bt_per_recv_params.ant_sel = packet[12];

  printf("--the type of the packet %d\n ", bt_per_recv_params.pkt_type);
  printf("--the address %x\n ",*(unsigned short int *)&bt_per_recv_params.bt_addr[4]);
  printf("--the address %x ",*(unsigned int *)&bt_per_recv_params.bt_addr[0]);

  memset(&bb_rf_params, 0, sizeof(bb_rf_params));
  memcpy(&bb_rf_params.Data[0], &bt_per_recv_params, sizeof(bt_per_recv_params));

  bb_rf_params.value = BT_RECEIVE;
  bb_rf_params.no_of_values = sizeof(bt_per_recv_params);
  if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER, sizeof(bb_rf_params)) < 0 )
  {
    printf("Unable to perform send stats request\n");
    return -1;
  }
  else
  {
    printf("receive Configurations sent successfully\n");
  }

  no_of_stats = packet[13];
  memset(&bb_rf_params, 0, sizeof(bb_rf_params));
  sta_info = (struct bt_per_stats_s*)malloc(100);

  while( no_of_stats )
  {
    loop_cnt = (no_of_stats / loop_chunk);
    if( !loop_cnt )
    {
      write_block = no_of_stats % loop_chunk;
      loop_cnt = 1;
    }
    else
    {
      write_block = loop_chunk;
    }
    for(loop_var =0; loop_var < loop_cnt; loop_var++ )
    {
      offset = 0;
      memset(send_rx_packet , 0 , loop_chunk * NO_OF_PARAMS*SIZE_OF_PARAM);
      for(ii=0; ii<write_block; ii++)
      {
        if(sleep(1)!=0)
        {
          memset(sta_info,0,sizeof(struct bt_per_stats_s));
          printf("Unable to sleep\n");            
          free(sta_info);        
          break;
        }

        bb_rf_params.value = BT_PER_STATS;

        if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_PER, sizeof(bb_rf_params)) < 0 )
        {
          printf("Unable to perform receive stats\n");
        }
        else
        {
          if( recv_data((uint_8*)sta_info) < 0 )
          {
            return -1;
          }
          if(first_time)
          {
            first_time = 0;
            free(sta_info);
            continue;
          }
          if((count % 20) == 0)
          {
            printf("%12s %12s %12s %12s %12s %12s %12s %12s %12s\n",
                "crc_fail","crc_pass","tx_aborts","cca_stk","cca_not_stk","fls_rx_start","cca_idle","tx_dones","rssi");
          }

          printf("%12d %12d %12d %12d %12d %12d %12d %12d %12d\n",
              *(unsigned short int *)&sta_info[0],
              *((unsigned short int *)&sta_info[0] + 1),
              *((unsigned short int *)&sta_info[0] + 2),
              *((unsigned short int *)&sta_info[0] + 3),
              *((unsigned short int *)&sta_info[0] + 4),
              *((unsigned short int *)&sta_info[0] + 5),
              *((unsigned short int *)&sta_info[0] + 6),
              *((unsigned short int *)&sta_info[0] + 7),
              *((signed short int *)&sta_info[0] + 8));

          local_buf[0] = htons(ii+1);   //(*(unsigned short *)&sta_info[2]);
          local_buf[1] = htons(*(unsigned short int *)&sta_info[0]);
          local_buf[2] = htons(*((unsigned short int *)&sta_info[0] + 1));
          local_buf[3] = htons(*((signed short int *)&sta_info[0] + 8));

          memcpy(&send_rx_packet[offset], local_buf, NO_OF_PARAMS * SIZE_OF_PARAM);
          offset = offset + NO_OF_PARAMS;

          ++count;    
        }    
      }
      onebox_send_packet(send_rx_packet, write_block*(NO_OF_PARAMS * SIZE_OF_PARAM));
      no_of_stats = no_of_stats - write_block;
    }
  }
    free(sta_info);
}
bt_ber_receive(uint_32 no_of_packets)
{
  int count = 0, stop_bit = 0;
  int i; 
  struct bt_ber_stats_s *bt_ber_info;
  struct iwreq iwr;
  struct bt_per_stats_s *sta_info;
  struct bb_rf_param_t bb_rf_params;

  struct bt_ber_pkt_cnt_s pkts_cnt;
  uint_8 send_bt_pkt[1500];
  uint_32 num_of_pkts;
  uint_16 failed_pkt_size = 1033;


  num_of_pkts  = no_of_packets;
  printf("Number_of_packets_to_transfer %d",num_of_pkts);
  memset(&bb_rf_params, 0,sizeof(bb_rf_params));
  bb_rf_params.value = BT_BER_PKT_CNT;
  bb_rf_params.Data[1] = num_of_pkts;

  if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_BER,sizeof(bb_rf_params)) < 0 )
  {
    printf("Unable to perform send stats request\n");
    return -1;
  }
  else
  {
    printf("Packet_count is updated\n");
  }

  sleep(1);
  bt_ber_info = (struct bt_ber_stats_s *)malloc(4096);
  if (!bt_ber_info)
    return -1;
  do
  {
    if(num_of_pkts == 0)
    {
      printf("Done : No packets to process\n");
      break;
    }

    memset(&bb_rf_params, 0, sizeof(bb_rf_params));
    bb_rf_params.value = BT_BER_RECEIVE;
    if( rsi_send_to_drv((uint_8 *)&bb_rf_params, BT_BER,sizeof(bb_rf_params)) < 0 )
    {
      printf("Unable to perform send stats request\n");
      free(bt_ber_info); //Free or no need?
      return -1;
    }
    if( recv_data((uint_8 *)bt_ber_info) < 0 )
    {
      num_of_pkts--;
      memset(send_bt_pkt,0,1500);
      send_bt_pkt[0] = no_of_packets - num_of_pkts;
      memcpy(&send_bt_pkt[1], &failed_pkt_size, 2); 
      onebox_send_packet((uint_16 *)send_bt_pkt, 1033);
      continue;
    }

    printf("parameter length %d\n", bt_ber_info->length);

    if( (bt_ber_info->length == 0) || (bt_ber_info->length >= 1500) )
    {
      num_of_pkts--;
      continue;
    }
    memset(send_bt_pkt,0,1500);
    //*(uint_16 *)&send_bt_pkt[0] = bt_ber_info->length;
    //memcpy(send_bt_pkt+2,bt_ber_info->data,bt_ber_info->length);
    memcpy(send_bt_pkt, bt_ber_info->data, bt_ber_info->length);

    //printf("%x\n", send_bt_pkt[0]);
    i = 0;

#if 1
    while(i < bt_ber_info->length)
    {
      //printf("0x%x\t", *(((unsigned short int*)&bt_ber_info->data + i)));
      printf("0x%x\t", (*(unsigned char*)&send_bt_pkt[i]));
      i = i + 1;
    }
#endif
    printf("\n");
    onebox_send_packet((uint_16 *)send_bt_pkt, 1033);

    num_of_pkts--;


  }while(1);

  free(bt_ber_info); //Free or no need?
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
    if( !read_len )
    {
      break;
    }
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

  /* Init netlink socket */
  if (rsi_netlink_init() < 0)
  {
    ONEBOX_PRINT ("Netling Socket creation error\n");
    return -1;
  }
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
  if (rsi_netlink_deinit() < 0)
  {
    ONEBOX_PRINT ("Netling Socket creation error\n");
    return -1;
  }
  close(sockfd);
  printf ("Device Closed\n");
  FUNCTION_EXIT();
  return 0;
}


