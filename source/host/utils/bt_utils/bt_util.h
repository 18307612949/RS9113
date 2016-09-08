/**
 * @file onebox_util.h
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
 * This file contians the structures and variables required for onebox_utility .
 */
#ifndef _BT_UTIL_H_
#define _BT_UTIL_H_

struct get_info {
	u_int8_t param_name[16];
	u_int8_t param_length;
	u_int8_t *data;
};

typedef struct bt_inf_s
{
	unsigned char hci_name[10];	
	unsigned char flag;
	struct iw_point	data;		/* Other large parameters */
}bt_inf_t;


typedef struct bt_per_stats_s
{
	unsigned short crc_fail;
	unsigned short crc_pass;
	unsigned short tx_aborts;
	unsigned short cca_stk;
	unsigned short cca_not_stk;
	unsigned short fls_rx_start;
	unsigned short cca_idle;
	unsigned short tx_dones;
	signed short int rssi;
	unsigned short id_pkts_rcvd;
}bt_per_stats_t;

typedef struct bt_ber_stats_s {
	unsigned short length;
	unsigned char data[1032];
	unsigned short num_pkts;
}__attribute__((packed)) bt_ber_stats_t;

typedef struct bt_ber_pkt_cnt_s{
		unsigned short num_pkts;
}__attribute__((packed)) bt_ber_pkt_cnt_t;





typedef struct  __attribute__((__packed__)) bt_per_params_s
{
	unsigned char  enable;
	unsigned char bt_addr[6];
	unsigned char pkt_type;
	unsigned short int  pkt_length;
	unsigned char link_type;
	unsigned char edr_indication;
	unsigned char bt_rx_rf_chnl;
	unsigned char bt_tx_rf_chnl;
	unsigned char scrambler_seed;
	unsigned int num_pkts;
	unsigned char payload_data_type;
	unsigned char mode;
	unsigned char le_chnl;
	unsigned char tx_pwr_indx;
	unsigned char transmit_mode;
	unsigned char enable_hopping;
	unsigned char ant_select;
#if 0
	unsigned int crc_init;
	unsigned int access_addr;
#endif
}bt_per_params_t;

typedef struct  __attribute__((__packed__)) bt_per_recv_params_s
{
	unsigned char  enable;
	unsigned char bt_addr[6];
	unsigned char link_type;
	unsigned char pkt_type;
	unsigned short int pkt_length;
	unsigned char scrambler_seed;
	unsigned char edr_indication;
	unsigned char bt_rx_rf_chnl;
	unsigned char bt_tx_rf_chnl;
  unsigned char le_mode;
  unsigned char le_chnl;
	unsigned char enable_hopping;
	unsigned char ant_sel;

}bt_per_recv_params_t;

typedef struct bt_per_packet_s
{
	unsigned char  enable;
	unsigned int  length;
	unsigned char packet[1024];
}bt_per_packet_t;


#define ONEBOX_PRINT(fmt, args...) fprintf(stdout, fmt, ## args)
#define ONEBOX_PRINT_INFO(a, fmt) \
	if(a)\
		printf(fmt);


/* Function prototypes */
void usage();
void byteconversion(char *src,char *macaddr);
int getcmdnumber(char *command);
void get_driver_state(char *state);
int bt_ber(void);
int bt_stats(unsigned char *);

inline void byteconversion (char *src, char *macaddr)
{
	int ii;
	unsigned char temp[18];

	for (ii = 0; *src != '\0'; ii++)
	{
		if (*src == ':')
		{
			src++;
		}
		if ((*src >= '0') && (*src <= '9'))
		{
			*src -= '0';
			temp[ii] = *src;
		}
		else if ((*src >= 'a') && (*src <= 'f'))
		{
			*src -= 'a';
			*src += 0xa;
			temp[ii] = *src;
		}
		else if ((*src >= 'A') && (*src <= 'F'))
		{
			*src -= 'A';
			*src += 0xa;
			temp[ii] = *src;
		}
		else
		{
			ONEBOX_PRINT ("Invalid macaddr\n");
			exit (1);
		}
		src++;
	}
  if( ii != 12 )
  {
    ONEBOX_PRINT ("Invalid macaddr\n");
    exit (1);
  }
	for (ii = 0; ii < 12;)
	{
		*macaddr = (temp[ii] << 4) | (temp[ii + 1]);
		ii = ii + 2;
		macaddr++;
	}
}


///***  MATLAB utils ****///

/*** Matlab To Driver Command Type Defines ***/

#define ONEBOX_STATUS_FAILURE           -1
#define ONEBOX_STATUS_SUCCESS           0
#define ONEBOX_STATUS                   int_32

#define ADDR0           0x315
#define ADDR1           0x316
#define ADDR2           0x317

#define BIT(n)         (1 << (n))

typedef unsigned char   uint_8;
typedef char            int_8;
typedef unsigned short  uint_16;
typedef short           int_16;
typedef unsigned int    uint_32;
typedef int             int_32;

/* Common Frame nos for matlab_utils.c and bt_util.c */
#define BB_READ                         0x0
#define BB_WRITE                        0x1
#define RF_READ                         0x2
#define RF_WRITE                        0x3
#define BT_PER_TRANSMIT                 0x4
#define BT_RECEIVE			                0x5
#define BUFFER_READ                     0x6
#define BUFFER_WRITE                    0x7
#define BT_PER_STATS 			              0x8
#define ANT_SEL			                    0x9
#define BT_BER_PKT_CNT		              0xA
#define BT_BER_RECEIVE                  0xB
#define BT_BER_MODE			                0xC
#define BT_CW_MODE                      0xD
#define TX_STATUS                       0xE
#define GET_DRV_COEX_MODE               0xF
#define BT_PER_AFH_MAP					0x10

#define BT_PER                          0x10
#define BT_BER                          0x11
#define BT_CW                           0x12

#define RSI_SET_BB_READ                 0x01
#define RSI_SET_BB_WRITE                0x02
#define RSI_RF_WRITE		                0x03    
#define RSI_RF_READ		                  0x04      
#define RSI_ANT_SEL		                  0x05
#define RSI_SET_CW_MODE                 0x06    
#define RSI_GET_BT_STATS                0x07
#define RSI_BT_BER                      0x08
#define RSI_SET_AFH_MAP					0x09

#define BT_CLASSIC                         1
#define BT_LE                              2


/*** Src Destn Port And IP Defines ***/
#define DESTN_PORT_NUM                  6666
#define SOURCE_PORT_NUM                 6666
#define DESTN_IP                        "192.168.70.222"
//#define DESTN_IP                        "127.0.0.1"
#define ONEBOX_MAX_PKT_LEN              6348 // Bytes
#define MAX_NUM_OF_PKTS                 100

#define IFNAMSIZ                        16


struct bb_rf_param_t
{
        unsigned char  value; //type is present here
        unsigned char  no_of_fields;
        unsigned char  no_of_values;
        unsigned char  soft_reset;
        unsigned short  Data[512];
} ;
     
#define FUNCTION_ENTRY()    NULL//            printf("+%s\n", __func__)
#define FUNCTION_EXIT()     NULL//            printf("-%s\n", __func__)

#endif


