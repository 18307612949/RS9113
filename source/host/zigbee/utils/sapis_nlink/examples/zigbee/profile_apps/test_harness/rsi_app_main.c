/**
 * @file    rsi_app_main.c
 * @version 0.1
 * @date    15 Sep 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains example application for TCP client socket
 *
 *  @section Description  This file contains example application for TCP client socket 
 *
 *
 */

/**
 * Include files
 * */
/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/socket.h>
#include "rsi_driver.h"
#include "rsi_framework.h"
#include "rsi_zb_api.h"
#include "AppTestHarness.h"
#include "buffer_management.h"
#include "rsi_zb_types.h"
#include "rsi_zb_interfaces.h"
#include "rsi_zb_config.h"
#include "ZCL_Interface.h"

uint32_t buffer_zdo[1000];

#define ZIGB_GLOBAL_BUFF_LEN    8000
#define COMMAND_BUFFER_SIZE     500
#define RSI_SUCCESS             0
#define g_BROADCAST_ADDRESS_c   0xFFFF
#define INADDR_ANY              0

/* returns an pointer to 64-bit extended address of the self device */
#define g_EXTENDED_PANID_c                {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

/* Application Events */
#define RSI_ZB_APP_TH_PKT_PENDING_EVENT            0x1 //0x1
#define RSI_ZB_APP_DATA_INDICATION_EVENT           0x2 //0x2
#define RSI_ZB_APP_INTERPAN_DATA_INDICATION_EVENT  0x4 //0x4
#define RSI_ZB_APP_SCAN_COMPLETE_HANDLER_EVENT	   0x8 //0x8
#define RSI_ZB_APP_STACK_STATUS_HANDLER_EVENT	   0x10 //0x16
#define RSI_ZB_APP_NWK_FOUND_HANDLER_EVENT	       0x20 //0x32
#define RSI_ZB_APP_DATA_CONFIRM_EVENT		       0x40 //0x64
#define RSI_ZB_ZDP_APPRESPONSE_EVENT		       0x80 //0x128

uint32_t rsi_app_async_event_map = 0;
uint8_t data_ind_buf_index;
uint8_t interpan_data_ind_buf_index;
uint8_t g_data_confirm_buf_index; 
uint8_t pass_msgdown_buf_index;

/*-----------------------------------------------------------------------------
 * Global Constants
 *----------------------------------------------------------------------------*/
uint8_t zigb_global_buf[ZIGB_GLOBAL_BUFF_LEN];

extern rsi_zigb_app_info_t rsi_zigb_app_info;
pthread_t  test_harness_handling_thread;
pthread_t  main_loop_thread;
pthread_t  timer_handling_thread;

/*-----------------------------------------------------------------------------
 * Public Memory declarations
 *******************************************************************************/
void rsi_zigb_event_init(void);
void rsi_zigb_SetEvent(uint32_t EventId);
void rsi_zigb_ClearEvent(uint32_t EventId);
uint32_t rsi_zigb_GetEvents();
static uint32_t Event_Bitmap;

/*-----------------------------------------------------------------------------
 * Socket Related declarations
 *----------------------------------------------------------------------------*/
char controller_ip[20];
int controller_port;
int ServerPortno;
int socket_descriptor;
int client_socket, SockWrite;
struct sockaddr_in servr_addr;
struct hostent *server;
struct sockaddr_in serv_addr, cli_addr;


extern void App_Handle_Data_Request(uint8_t msgId,uint8_t msg_index);
extern void Application_to_ZDO ( uint8_t msg_type, uint8_t msg_index );
extern void App_Stack_Information ( uint8_t msg_type, uint8_t msg_index );

/* Extern declaration of pointers for ZDO Information Base */
ZDO_Information_Base_t *gp_ZDO_Information_Base;
uint8_t *ga_Stack_Persistent_Data_Sector_One;
uint32_t zdoInfoBaseNVMLocation;
uint32_t g_NVM_Tables_Start_Offset_c;
APSDE_Data_Indication_t DataIndication;

#if (g_APP_PROFILE_ID_c==0xc05e)
ZDO_Configuration_Table_t gp_ZDO_Configuration[];
Startup_Attribute_Set_t gp_Active_Startup_Attrib_Set[];
#endif
/*******************************************************************************
 * Public Functions
 *******************************************************************************/
#ifdef DEBUG_DUMP_BYTES
void DebugDumpBytes(unsigned char *buffer, unsigned short length, char *pDescription)
{
	char stream[60];
	char byteOffsetStr[10];
	unsigned long i;
	unsigned short offset, count, byteOffset;

	printf("<---------Dumping %d Bytes : %s ------>\n", length,pDescription);
	count = 0;
	offset = 0;
	byteOffset = 0;
	for (i = 0; i < length; i++)
	{
		sprintf(stream + offset, "%2.2X ", buffer[i]);
		count ++;
		offset += 3;

		if (count == 16)
		{
			count = 0;
			offset = 0;
			sprintf(byteOffsetStr,"%4.4X",byteOffset);
			printf("[%s]: %s\n", byteOffsetStr, stream);
			memset(stream,0, 60);
			byteOffset += 16;
		}
	}

	if (offset != 0)
	{
		sprintf(byteOffsetStr,"%4.4X",byteOffset);
		printf("[%s]: %s\n", byteOffsetStr, stream);
	}
	printf("<------------------------------------------------->\n");
}
#endif
/*==============================================*/
/**
 * @fn         rsi_zb_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BLE initialization.
 */
static void rsi_zb_app_init_events()
{
	rsi_app_async_event_map = 0;
	return;
}

/*==============================================*/
/**
 * @fn         rsi_zb_app_set_event
 * @brief      set the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_zb_app_set_event(uint32_t event_num)
{
	rsi_app_async_event_map |= BIT(event_num);
	return;
}

/*==============================================*/
/**
 * @fn         rsi_zb_app_clear_event
 * @brief      clear the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_zb_app_clear_event(uint32_t event_num)
{
	rsi_app_async_event_map &= ~BIT(event_num);
	return;
}

/*==============================================*/
/**
 * @fn         rsi_zb_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_zb_app_get_event(void)
{
	uint32_t  ix;

	for (ix = 0; ix < 32; ix++)
	{
		if (rsi_app_async_event_map & (1 << ix))
		{
		   return ix;
		}
	}

	return (-1);
}

void rsi_zb_app_handle_data_confirm_event(void)
{
	APSDE_Data_Confirmation_t *pDataConfirmation = (APSDE_Data_Confirmation_t *)buffMgmt_getBufferPointer(g_data_confirm_buf_index);
	
	AppHandleDataConfirmation(g_data_confirm_buf_index,pDataConfirmation);

	return;
}

void rsi_zb_app_handle_data_indication_event(void)
{
  APSDE_Data_Indication_t *pDataIndication = (APSDE_Data_Indication_t *)buffMgmt_getBufferPointer(data_ind_buf_index);

  AppHandleDataIndication(data_ind_buf_index,pDataIndication);

  return;
}

void rsi_zb_app_handle_interpan_data_indication_event(void)
{
  APSDE_InterPan_Data_Indication_t *pApsdeDataInd = (APSDE_InterPan_Data_Indication_t *)buffMgmt_getBufferPointer(interpan_data_ind_buf_index);

  App_InterPanDataindication(interpan_data_ind_buf_index,pApsdeDataInd);


  return;  
}

void rsi_zb_app_scan_complete_handler_event(void)
{
  AppScanCompleteHandler(rsi_zigb_app_info.scan_done_cb.channel, rsi_zigb_app_info.scan_done_cb.scan_status);

  return;
}

void rsi_zb_nwk_found_handler_event(void)
{
  AppNetworkFoundHandler(rsi_zigb_app_info.nwkinfo);  

  return;
}

void rsi_zb_app_stack_status_handler_event(void)
{
  AppZigBeeStackStatusHandler(&(rsi_zigb_app_info.stack_status));

  return;
}

void InitgpZDOInfoBase( uint32_t ZDOInfoBaseOffset )
{
	gp_ZDO_Information_Base = (ZDO_Information_Base_t*)
							  (ga_Stack_Persistent_Data_Sector_One +
							   ZDOInfoBaseOffset );
	zdoInfoBaseNVMLocation = ZDOInfoBaseOffset +g_NVM_Tables_Start_Offset_c;
}

void main_loop(void)
{
	printf("\nmain loop function \n");
	while (1)
	{
		rsi_non_os_event_loop();
		usleep(100);
	}
	printf("\nAfter the while1 \n");
}
uint8_t *pass_msgdown_ptr=NULL;
void test_harness_pkt_handler()
{
	int sockfd, newsockfd,i,on;
	socklen_t clilen;
	int receive_count = 0;   
	unsigned char *rx_buffer;
	char buffer[COMMAND_BUFFER_SIZE];

	while(1)
	{
		printf("\n**************************************************************\n");
		printf("*                                                            *\n");
		printf("*            Redpine Zigbe Profile Validation Setup          *\n"); 
		printf("*                                                            *\n");
		printf("**************************************************************\n");

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
		{
			printf("ERROR opening socket");
		}

		bzero((char *) &serv_addr, sizeof(serv_addr));
		/* Enable address reuse */
		on = 1;
		setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(ServerPortno);
		if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
		{
			printf("ERROR on binding\n");
		}
		printf("Listening on port %d\n",ServerPortno);
		listen(sockfd,1);

		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
		sprintf(controller_ip,"%s", inet_ntoa(cli_addr.sin_addr));
		memcpy(controller_ip,(char *) inet_ntoa(cli_addr.sin_addr),sizeof(inet_ntoa(cli_addr.sin_addr)));
		printf("Established Connection with clinet IP: %s,Port: %d\n",controller_ip,ntohs(cli_addr.sin_port));

		if (newsockfd < 0)
		{
			printf("ERROR on accept");
		}
		connect_with_client();

		while (1)
		{
			bzero(buffer,COMMAND_BUFFER_SIZE);
			socket_descriptor= newsockfd;

			receive_count = read(newsockfd,buffer,10000);
			if (receive_count < 0)
			{
				close(sockfd);
				close(newsockfd);
				close(client_socket);
				printf("Error in Reading from Socket\n");
				printf("Received socket close request from client \n");
				break;

			}
			if (receive_count==0)
			{
				close(sockfd);
				close(newsockfd);
				close(client_socket);
				break;
			}
			
			pass_msgdown_buf_index = buffMgmt_allocateBuffer(g_LARGE_BUFFER_c);

			pass_msgdown_ptr = buffMgmt_getBufferPointer(pass_msgdown_buf_index);

			if(pass_msgdown_buf_index == g_NO_AVAILABLE_BUFFER_c)
  			{
   				printf("\n Unable to handle Data Indication: Buffer Not Available \n");
				return;
			}
			
			memcpy(pass_msgdown_ptr,buffer,g_LARGE_BUFFER_c);
	
			rsi_zb_app_set_event(RSI_ZB_APP_TH_PKT_PENDING_EVENT);
			/* Here buffer has to send to the stack   */

			// Send_response( buffer );      /* For temporary verification */
			// buffMgmt_freeBuffer( buffer_id, 144);
			bzero(buffer,COMMAND_BUFFER_SIZE);
			usleep(10);
		}
	}
}

/******************************************************************************************************/
extern uint32_t currentTime;
extern uint32_t currentTick;
uint8_t stack_is_up;
void CheckFreeRunTimer(void)
{
    while(1)
    {
        sleep(1);
        currentTime++;
        currentTick++;
    }
}
/******************************************************************************************************/
int32_t setup_test_harness(void)
{
	int32_t status = RSI_SUCCESS;
	uint8_t device_type = 2;
	uint8_t   ieee_addr[8]={0x0};
	uint8_t   read_ieee_addr[8]={0x0};
	uint8_t   i=0;
	uint16_t nwk_addr_of_interest = 0x0000;
	uint16_t dest_addr = g_BROADCAST_ADDRESS_c;
	uint16_t* lightAddress;
	rsi_zigb_app_info_t   *app_info = &rsi_zigb_app_info;
	rsi_zigb_cb_t *app_cb_ptr = rsi_driver_cb->zigb_cb;
	int rc1;
	uint8_t timer_thread;
	uint8_t *th_ptr = "Timer Thread";
	char *message1 = "Recv Thread";


	//! Driver initialization
	status = rsi_driver_init(zigb_global_buf, ZIGB_GLOBAL_BUFF_LEN);
	if(status != RSI_SUCCESS)
	{
		RSI_DPRINT(RSI_PL1,"\n driver init failed");
		return status;
	}
#ifdef UART_INTERFACE
	//rsi_uart_init();
#endif

	//! WC initialization
	//status = rsi_wireless_init(0, 3);
 	rsi_bypass_opmode();
	if(status != RSI_SUCCESS)
	{
		return status;
	}	

	Test_Harness_Data_Path = App_Handle_Data_Request;
	Test_Harness_Management_Path = Application_to_ZDO;
	Test_Harness_To_Test_Utility = App_Stack_Information;
	
	TMR_Init();

	buffMgmt_init();

	App_Init();
	InitgpZDOInfoBase((unsigned long) buffer_zdo);
	rsi_zigb_register_callbacks (
			rsi_zigb_app_scan_complete_handler,
			rsi_zigb_app_energy_scan_result_handler,             
			rsi_zigb_app_network_found_handler,
			rsi_zigb_app_stack_status_handler,
			rsi_zigb_app_incoming_many_to_one_route_req_handler,
			rsi_zigb_app_handle_data_indication,
			rsi_zigb_app_handle_data_confirmation,
			rsi_zigb_app_child_join_handler,
			rsi_zigb_nvm_backup_handler,
			rsi_zigb_zdp_app_response_handler

#ifdef ZB_PROFILE
			,rsi_zigb_app_interpan_data_indication,
			rsi_zigb_app_interpan_data_confirmation
#endif
			);
	//! zigb stack init 
	status = rsi_zigb_init_stack();

	//! zigb stack reset 
	status = rsi_zigb_reset_stack();


	rsi_zigb_enable_profile_test(1);

	rsi_zigb_set_profile(1);

	//scanf("%d",&status);
	status = rsi_zigb_get_device_type(&device_type);
	RSI_DPRINT(RSI_PL1,"\nDevice:%X\n", device_type);


	if ( (rc1 = pthread_create( &test_harness_handling_thread, NULL, test_harness_pkt_handler, (void*) message1)) )
	{
		printf("Test Harness Thread creation failed: %d\n", rc1);
		return -1;
	}
	/* if ( (rc1 = pthread_create( &main_loop_thread, NULL, main_loop, (void*) message1)) )
	   {
	   printf("Test Harness Thread creation failed: %d\n", rc1);
	   return -1;
	   }*/
	if ( ( timer_thread = pthread_create(&timer_handling_thread,NULL,CheckFreeRunTimer,(void *)th_ptr)))
	{
	    printf("Timer Thread creation failed: %d\n", timer_thread);
	    return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int status;
	uint32_t temp_event_map = 0;

	if (argc < 2)
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	ServerPortno = atoi(argv[1]);

	status = setup_test_harness();

	if (status != 0)
	{
		fprintf(stderr, "Test Harness Initialization Failed \n");
		exit(2);
	}

	while (1)
	{
		App_Main();

		Test_Main();

#if ( g_APP_PROFILE_ID_c == 0xc05e )
		ZLL_Tick();
#endif
		//! Driver Task
		rsi_non_os_event_loop();

		//! Check the updated Event Map
		temp_event_map = rsi_zb_app_get_event();

		switch (temp_event_map)
		{
			case RSI_ZB_APP_TH_PKT_PENDING_EVENT:
			{

				printf("Buffer ID: %d Address :%X\n",pass_msgdown_buf_index,pass_msgdown_ptr);
#ifdef DEBUG_DUMP_BYTES
				DebugDumpBytes(pass_msgdown_ptr,g_LARGE_BUFFER_c,"Command Received");
#endif
				testHrns_passMessageDown(pass_msgdown_buf_index);
				
				//! Clear the Event
				rsi_zb_app_clear_event(RSI_ZB_APP_TH_PKT_PENDING_EVENT);
			}
			break;
			//! Checking For Driver Event 
			case RSI_ZB_APP_DATA_INDICATION_EVENT:
			{
				//! Data Indication Event Handling
				rsi_zb_app_handle_data_indication_event();

				//! Clear the Event
				rsi_zb_app_clear_event(RSI_ZB_APP_DATA_INDICATION_EVENT);
			}
			break;
			case RSI_ZB_APP_INTERPAN_DATA_INDICATION_EVENT:
			{
				//!  Interpan Data Indication Event Handling
				rsi_zb_app_handle_interpan_data_indication_event();

				//! Clear the Event
				rsi_zb_app_clear_event(RSI_ZB_APP_INTERPAN_DATA_INDICATION_EVENT);
			}
			break;
			case RSI_ZB_APP_SCAN_COMPLETE_HANDLER_EVENT:
			{
				//!  Scan Complete Event Handling
				rsi_zb_app_scan_complete_handler_event();

				//! Clear the Event
				rsi_zb_app_clear_event(RSI_ZB_APP_SCAN_COMPLETE_HANDLER_EVENT);
			}
			break;
			case RSI_ZB_APP_STACK_STATUS_HANDLER_EVENT:
			{
				//! Stack Status Event Handling
				rsi_zb_app_stack_status_handler_event();

				//! Clear the Event
				rsi_zb_app_clear_event(RSI_ZB_APP_STACK_STATUS_HANDLER_EVENT);
			}
			break;
			case RSI_ZB_APP_NWK_FOUND_HANDLER_EVENT:
			{
				//! NWK Found Handler Event
				rsi_zb_nwk_found_handler_event();

				//! Clear the Event
				rsi_zb_app_clear_event(RSI_ZB_APP_NWK_FOUND_HANDLER_EVENT);
			}
			break;
			case RSI_ZB_APP_DATA_CONFIRM_EVENT:
			{
				//! DATA Complete Handler Event
				rsi_zb_app_handle_data_confirm_event();

				//!  Clear the Event
				rsi_zb_app_clear_event(RSI_ZB_APP_DATA_CONFIRM_EVENT);
			}
			break;
		}
	usleep(10);
	}
}

int connect_with_client()
{
	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (client_socket < 0)
	{
		printf("ERROR opening socket");
	}
	controller_port = ServerPortno +10;
	printf("controller port is %d\n",controller_port);
	server = gethostbyname(controller_ip);
	if (server == NULL)
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &servr_addr, sizeof(servr_addr));
	servr_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr_list[0], (char *)&servr_addr.sin_addr.s_addr,server->h_length);
	servr_addr.sin_port = htons(controller_port);
	if (connect(client_socket,(struct sockaddr *) &servr_addr,sizeof(servr_addr)) < 0)
	{
		printf("ERROR connecting");
	}
	else
	{
		printf("Connected with Client IP: %s:Port:%d\n",controller_ip,controller_port);
	}

}
int send_response(  char *response )	  /* call this function, when the response has to send to host */
{
	printf("Response Send    : %s\n",response);

	SockWrite = write(client_socket,response,strlen(response)); 
	/*  Writing  Zigbee command to the Module    */

	if (SockWrite < 0)
	{
		printf("ERROR writing to socket");
		close(client_socket);
	}
	return 0;
}

void send_response_to_client(unsigned char *bufferTx,int length)
{
#ifdef DEBUG_DUMP_BYTES
	DebugDumpBytes(bufferTx,length,"Response Sent");
#endif
	SockWrite = write(client_socket,bufferTx,length); 
	if (SockWrite < 0)
	{
		printf("ERROR writing to socket");
		close(client_socket);
	}
	return 0;

}

//! Event callbacks
/*===========================================================================
 *
 * @fn          void rsi_zigb_app_scan_complete_handler (uint32_t channel, 
 *                                                       uint8_t status )
 * @brief       Scan complete handler 
 * @param[in]   Channel
 * @param[in]   Status of channel whether beacons are found
 * @return      none
 * @section description
 * This API is used to handle ZigBee scan complete state
 * It provides infromation of the channel whether beacons are found or not
 * Updating few app_info variables 
 *
 * ===========================================================================*/
  
void rsi_zigb_app_scan_complete_handler ( uint32_t channel, uint8_t status )
{
  rsi_zigb_app_info.scan_done_cb.channel = channel; 
  rsi_zigb_app_info.scan_done_cb.scan_status = status; 
  rsi_zigb_app_info.status_var.scanReqSent = 0;
  if(!status)
  {
   rsi_zigb_app_info.state = FSM_SCAN_COMPLETE; 
  }
  RSI_DPRINT(RSI_PL1,"\nAppScanCompleteHandler");
#ifdef ZB_DEBUG  
  RSI_DPRINT(RSI_PL1,"\nScan Status = %x \n", status);
#endif  
  rsi_zb_app_set_event(RSI_ZB_APP_SCAN_COMPLETE_HANDLER_EVENT);
}

/*===========================================================================
 *
 * @fn          void rsi_zigb_app_energy_scan_result_handler( uint32_t channel,
 *                                                       uint8_t *pEnergyValue)
 * @brief       Energy Scan complete handler 
 * @param[in]   Channel
 * @param[in]   Energy Value (RSSI)
 * @return      none
 * @section description
 * This API is used to handle ZigBee Energy scan complete state
 * Here Energy in each channel is received, for the provided channels 
 * issued by user to scan
 *
 * ===========================================================================*/
void rsi_zigb_app_energy_scan_result_handler( uint32_t channel,uint8_t *pEnergyValue)
{
}

/*===========================================================================
 *
 * @fn          void rsi_zigb_app_network_found_handler(ZigBeeNetworkDetails)
 * @brief       Network found indication handler 
 * @param[in]   NetworkInformation data 
 * @param[out]  none
 * @return      none
 * @section description
 * This API is used to handle network found indication frame  
 * Infromation about the found network is updated 
 *
 * ===========================================================================*/

void rsi_zigb_app_network_found_handler(ZigBeeNetworkDetails networkInformation)
{
  uint8_t i=0;
  ZigBeeNetworkDetails *nwk_details = &(rsi_zigb_app_info.nwkinfo);
  /* Currently we are checking for any coordinator, if you know the specific 
   * extended panid, then check here for specific panid */
  rsi_zigb_mcpy((uint8_t *)&networkInformation, (uint8_t *)nwk_details, sizeof(ZigBeeNetworkDetails));
  RSI_DPRINT(RSI_PL1,"\nAppNetworkFoundHandler \n ");
#ifdef ZB_DEBUG  
  RSI_DPRINT(RSI_PL1,"\nchannel: %d  ",networkInformation.channel);
  RSI_DPRINT(RSI_PL1,"\nshort panid: %d  ",networkInformation.shortPanId);
  printf("\n ## Extended panid");
  for(i=0;i<8;i++)
  {
    printf(" 0x%x",networkInformation.extendedPanId[i]);
  }
  
#endif  
  rsi_zb_app_set_event(RSI_ZB_APP_NWK_FOUND_HANDLER_EVENT);
}

/*===========================================================================
 *
 * @fn          void rsi_zigb_app_stack_status_handler(ZigBeeNWKStatusInfo *statusInfo)
 * @brief       Stack status Indication
 * @param[in]   Network status Information 
 * @param[out]  none
 * @return      none
 * @section description
 * This API is used to handle network/stack status
 * Infromation about network status (If connection successful of failed) 
 *
 * ===========================================================================*/

void rsi_zigb_app_stack_status_handler(ZigBeeNWKStatusInfo *statusInfo)
{

	rsi_zigb_app_info.stack_status = *statusInfo;
	
	{
		rsi_zigb_app_info.state = FSM_JOIN_COMPLETE; 
	}
	rsi_zigb_app_info.state = ZigBeeNWKIsUp;
#ifdef ZB_DEBUG  
  RSI_DPRINT(RSI_PL1,"\n Stack Status = %x \n", *statusInfo);
#endif 
        stack_is_up = 1;
	rsi_zb_app_set_event(RSI_ZB_APP_STACK_STATUS_HANDLER_EVENT); 
}

/*===========================================================================
 *
 * @fn          void rsi_zigb_app_incoming_many_to_one_route_req_handler(uint16_t SourceAddr,
 *                                                  uint8_t * pSrcIEEEAddr,uint8_t PathCost )
 * @brief       Many to one route request handler
 * @param[in]   Source short Addr
 * @param[in]   Source IEEE address
 * @param[in]   Path cost
 * @param[out]  none
 * @return      none
 * @section description
 * This API is used to handle Many to one route request
 * We have to decide which route to accept based on path cost 
 *
 * ===========================================================================*/

void rsi_zigb_app_incoming_many_to_one_route_req_handler( uint16_t SourceAddr, uint8_t * pSrcIEEEAddr,uint8_t PathCost )
{
#ifdef ZB_DEBUG  
  RSI_DPRINT(RSI_PL1,"\n Called rsi_zigb_app_incoming_many_to_one_route_req_handler \n ");
  RSI_DPRINT(RSI_PL1,"\n SorceAddr: 0x%x",SourceAddr);
  RSI_DPRINT(RSI_PL1,"\n PathCost: %x",PathCost);
#endif  
}

/*===========================================================================
 *
 * @fn          void rsi_zigb_app_handle_data_indication(
 *                                   APSDE_Data_Indication_t * pDataIndication )
 * @brief       Handle data indication frame
 * @param[in]   Data indication info struct 
 * @param[out]  none
 * @return      none
 * @section description
 * This API is used to handle received data indication frame
 *
 * ===========================================================================*/

void rsi_zigb_app_handle_data_indication(APSDE_Data_Indication_t *pDataIndication)
{
#if 0  
  uint8_t buffer_index;
  int i = 0;
  rsi_zigb_app_info_t *app_info = &rsi_zigb_app_info;
  RSI_DPRINT(RSI_PL1,"\nData Indication");
#if 1//ZB_DEBUG
  RSI_DPRINT(RSI_PL1,"\nData Indication");
  RSI_DPRINT(RSI_PL1,"\n pDataIndication->cluster_id: 0x%x",pDataIndication->cluster_id);
  RSI_DPRINT(RSI_PL1,"\n status: 0x%x",pDataIndication->a_asdu[1]);
  RSI_DPRINT(RSI_PL1,"\n Source EP: 0x%x",pDataIndication->src_endpoint);
  RSI_DPRINT(RSI_PL1,"\n Source A M: 0x%x",pDataIndication->src_addr_mode);
  RSI_DPRINT(RSI_PL1,"\n Source Addr: 0x%x",pDataIndication->src_address);
  RSI_DPRINT(RSI_PL1,"\n profile_id: 0x%x",pDataIndication->profile_id);
  RSI_DPRINT(RSI_PL1,"\n cluster_id: 0x%x\n",pDataIndication->cluster_id);
  RSI_DPRINT(RSI_PL1,"\n Dest A M  : 0x%x\n",pDataIndication->dest_addr_mode);
  RSI_DPRINT(RSI_PL1,"\n Dest Addr : 0x%x\n",pDataIndication->dest_address);
  RSI_DPRINT(RSI_PL1,"\n Dest E P  : 0x%x\n",pDataIndication->dest_endpoint);
  RSI_DPRINT(RSI_PL1,"\n Was broad : 0x%x\n",pDataIndication->was_broadcast);
  RSI_DPRINT(RSI_PL1,"\n asdu len  : 0x%x\n",pDataIndication->asdulength);
  RSI_DPRINT(RSI_PL1,"\n Secu statu: 0x%x\n",pDataIndication->security_status);
  RSI_DPRINT(RSI_PL1,"\n Link Quali: 0x%x\n",pDataIndication->link_quality);

#endif
    buffer_index = buffMgmt_allocateBuffer ( g_LARGE_BUFFER_c );
   if( buffer_index != g_NO_AVAILABLE_BUFFER_c )
  {
     printf("\n Buffer not available");
  }
  AppHandleDataIndication(buffer_index,pDataIndication);
#else
  data_ind_buf_index = buffMgmt_allocateBuffer(g_LARGE_BUFFER_c);

  APSDE_Data_Indication_t *DataIndication = (APSDE_Data_Indication_t *)buffMgmt_getBufferPointer(data_ind_buf_index);

  if(data_ind_buf_index == g_NO_AVAILABLE_BUFFER_c)
  {
     printf("\n Unable to handle Data Indication: Buffer Not Available \n");
     return;
  }
  
  if(pDataIndication->profile_id != g_APP_PROFILE_ID_c)
  {
        buffMgmt_freeBuffer( data_ind_buf_index,161 );	
	return;
  }
  DataIndication->dest_addr_mode = pDataIndication->dest_addr_mode;
  if (DataIndication->dest_addr_mode == 0x3) {
    memUtils_memCopy(DataIndication->dest_address.IEEE_address, pDataIndication->dest_address.IEEE_address, sizeof(Address));
  } else {
    DataIndication->dest_address.short_address = pDataIndication->dest_address.short_address;
  }

  DataIndication->dest_endpoint = pDataIndication->dest_endpoint;
  DataIndication->src_addr_mode = pDataIndication->src_addr_mode;
  if (DataIndication->src_addr_mode == 3) {
    memUtils_memCopy(DataIndication->src_address.IEEE_address, pDataIndication->src_address.IEEE_address, sizeof(Address));
  } else {
    DataIndication->src_address.short_address = pDataIndication->src_address.short_address;
  }
  DataIndication->src_endpoint = pDataIndication->src_endpoint;
  DataIndication->profile_id = pDataIndication->profile_id;
  DataIndication->cluster_id = pDataIndication->cluster_id;
  DataIndication->asdulength = pDataIndication->asdulength;
  DataIndication->was_broadcast = pDataIndication->was_broadcast;
  DataIndication->security_status = pDataIndication->security_status;
  DataIndication->link_quality = pDataIndication->link_quality;
  memUtils_memCopy(DataIndication->a_asdu, pDataIndication->a_asdu, DataIndication->asdulength);

  rsi_zb_app_set_event(RSI_ZB_APP_DATA_INDICATION_EVENT);
#endif  
  return;
}
/*===========================================================================
 *
 * @fn          void rsi_zigb_app_handle_data_confirmation (
 *                                   APSDE_Data_Confirmation_t* pDataConfirmation )
 * @brief       Handle data confirmation frame
 * @param[in]   Buffer Index of actual data from the pointer
 * @param[in]   Data confirmation info struct 
 * @param[out]  none
 * @return      none
 * @section description
 * This API is used to handle received data confirmation frame for the 
 * data request sent
 *
 * ===========================================================================*/
uint32_t dataConfcnt;

APSDE_Data_Confirmation_t *APSDE_Data_Confir = NULL;

void rsi_zigb_app_handle_data_confirmation (APSDE_Data_Confirmation_t *pDataConfirmation)
{
	g_data_confirm_buf_index =  buffMgmt_allocateBuffer(g_LARGE_BUFFER_c);

	APSDE_Data_Confir = (APSDE_Data_Confirmation_t *)buffMgmt_getBufferPointer(g_data_confirm_buf_index);

	if(g_data_confirm_buf_index == g_NO_AVAILABLE_BUFFER_c)
  	{
            printf("\n Unable to handle Data Confirmation: Buffer Not Available \n");
    	    return;
  	}
	APSDE_Data_Confir->dest_addr_mode = pDataConfirmation->dest_addr_mode;
	if(APSDE_Data_Confir->dest_addr_mode == 0x03)
	{
	   memUtils_memCopy(APSDE_Data_Confir->dest_address.IEEE_address, pDataConfirmation->dest_address.IEEE_address, sizeof(Address));
        }
	else 
	{
	   APSDE_Data_Confir->dest_address.short_address = pDataConfirmation->dest_address.short_address;
  	}
	APSDE_Data_Confir->dest_endpoint  = pDataConfirmation->dest_endpoint;
	APSDE_Data_Confir->src_endpoint	  = pDataConfirmation->src_endpoint;
	APSDE_Data_Confir->status         = pDataConfirmation->status;
	AppHandleDataConfirmation(g_data_confirm_buf_index,APSDE_Data_Confir);	
//	rsi_zb_app_set_event(RSI_ZB_APP_DATA_CONFIRM_EVENT);	
}

/*===========================================================================
 *
 * @fn          void rsi_zigb_app_child_join_handler(uint16_t short_address,
 *                                                   BOOL joining)
 * @brief       Child join handler 
 * @param[in]   Short_addr of child
 * @param[in]   Status of child joining/leaving 
 * @return      none
 * @section description
 * This API is used to handle child join/leave status
 *
 * ===========================================================================*/

void rsi_zigb_app_child_join_handler(uint16_t short_address, char joining)
{
#ifdef ZB_DEBUG  
  RSI_DPRINT(RSI_PL1,"\n Called rsi_zigb_app_child_join_handler \n ");
  RSI_DPRINT(RSI_PL1,"ShortAddr: 0x%x",short_address);
  RSI_DPRINT(RSI_PL1,"\n Joining: %x\n",joining);
#endif  
}
//////////////////////////////////////////////////////
APSDE_InterPan_Data_Indication_t *Interpan_DataIndication=NULL;

void rsi_zigb_app_interpan_data_indication( uint8_t bufferid, APSDE_InterPan_Data_Indication_t *pApsdeDataInd )
{

  interpan_data_ind_buf_index = buffMgmt_allocateBuffer(g_LARGE_BUFFER_c);

 // APSDE_InterPan_Data_Indication_t *Interpan_DataIndication = (APSDE_InterPan_Data_Indication_t *)buffMgmt_getBufferPointer(interpan_data_ind_buf_index);
  Interpan_DataIndication = (APSDE_InterPan_Data_Indication_t *)buffMgmt_getBufferPointer(interpan_data_ind_buf_index);


  if(interpan_data_ind_buf_index == g_NO_AVAILABLE_BUFFER_c)
  {
     printf("\n Unable to handle Data Indication: Buffer Not Available \n");
     return;
  }

  Interpan_DataIndication->dstaddrmode = pApsdeDataInd->dstaddrmode;
  if (Interpan_DataIndication->dstaddrmode == 0x3) {
    memUtils_memCopy(Interpan_DataIndication->dstAddress.IEEE_address, pApsdeDataInd->dstAddress.IEEE_address, sizeof(Address));
  } else {
    Interpan_DataIndication->dstAddress.short_address = pApsdeDataInd->dstAddress.short_address;
  }

  Interpan_DataIndication->srcaddrmode = pApsdeDataInd->srcaddrmode;
  if (Interpan_DataIndication->srcaddrmode == 3) {
    memUtils_memCopy(Interpan_DataIndication->srcaddress.IEEE_address, pApsdeDataInd->srcaddress.IEEE_address, sizeof(Address));
  } else {
    Interpan_DataIndication->srcaddress.short_address = pApsdeDataInd->srcaddress.short_address;
  }
  Interpan_DataIndication->profileId = pApsdeDataInd->profileId;
  Interpan_DataIndication->clusterId = pApsdeDataInd->clusterId;
  Interpan_DataIndication->srcpanid  = pApsdeDataInd->srcpanid;
  Interpan_DataIndication->dstpanid  = pApsdeDataInd->dstpanid;
  Interpan_DataIndication->asdulength = pApsdeDataInd->asdulength;
  Interpan_DataIndication->link_quality = pApsdeDataInd->link_quality;
  memUtils_memCopy(Interpan_DataIndication->a_asdu, pApsdeDataInd->a_asdu, Interpan_DataIndication->asdulength);

  rsi_zb_app_set_event(RSI_ZB_APP_INTERPAN_DATA_INDICATION_EVENT);
//  rsi_zb_app_handle_interpan_data_indication_event();
  buffMgmt_freeBuffer( bufferid,161 );
}

////////////////////////////////////////////////////////////////////////////////////////

void rsi_zigb_app_interpan_data_confirmation( uint8_t bufferid, APSDE_InterPan_Data_Confirmation_t *pInterPanDataConf )
{
#if (g_APP_PROFILE_ID_c==0xc05e)
  App_InterPanDataConfirmation(bufferid,pInterPanDataConf);
#endif
}

void rsi_zigb_zdp_app_response_handler(uint8_t bufferIndex)
{
   /*  uint8_t buf_id=0;
     uint8_t *buf_ptr=NULL;

     buf_id = buffMgmt_allocateBuffer(g_LARGE_BUFFER_c);
     if ( buf_id != 0xFF ) {
        buf_ptr = buffMgmt_getBufferPointer(buf_id);
        memUtils_memCopy(buf_ptr,buffer,g_LARGE_BUFFER_c);*/
        ZDP_APPresponse(bufferIndex);
   //  }
}

void rsi_zigb_nvm_backup_handler(void *data, uint32_t offset)
{
  uint16_t SASIndex = 0;
  uint16_t length=0;
  rsi_zigb_app_info_t   *app_info = &rsi_zigb_app_info;
  SASIndex= app_info->nvm_storage.sas_index;
  printf("\n rsi_zigb_nvm_backup_handler");
  if(offset == g_NVM_SAS_TABLE_START_OFFSET_c)
  {
    length = 2;
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.sas_index,data,length);
    SASIndex = app_info->nvm_storage.sas_index;
  }

  if(offset == (g_NVM_SAS_TABLE_START_OFFSET_c + 2))
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.gp_ZDO_Configuration,data,sizeof(app_info->nvm_storage.gp_ZDO_Configuration));
  }

  if(offset == ((g_NVM_SAS_TABLE_START_OFFSET_c) + ((SASIndex + 1) * m_SIZE_CONFIGURATION_c)))
  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.sas_data,data,m_SIZE_CONFIGURATION_c);
  }

  if(offset == ((g_NVM_SAS_TABLE_START_OFFSET_c) + ((SASIndex + 2) * m_SIZE_CONFIGURATION_c)))
  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.sas_data[1],data,m_SIZE_CONFIGURATION_c);
  }

  if(offset == ((g_NVM_SAS_TABLE_START_OFFSET_c) + ((SASIndex + 3) * m_SIZE_CONFIGURATION_c)))
  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.sas_data[2],data,m_SIZE_CONFIGURATION_c);
  }

  if(offset == ((g_NVM_SAS_TABLE_START_OFFSET_c) + ((SASIndex + 4) * m_SIZE_CONFIGURATION_c)))

  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.sas_data[3],data,m_SIZE_CONFIGURATION_c);
  }

  if(offset == ((g_NVM_SAS_TABLE_START_OFFSET_c) + ((SASIndex + 5) * m_SIZE_CONFIGURATION_c)))

  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.sas_data[4],data,m_SIZE_CONFIGURATION_c);
  }

  if(offset == ((g_NVM_SAS_TABLE_START_OFFSET_c) + ((SASIndex + 6) * m_SIZE_CONFIGURATION_c)))

  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.sas_data[5],data,m_SIZE_CONFIGURATION_c);
  }

  if(offset == ((g_NVM_SAS_TABLE_START_OFFSET_c) + ((SASIndex + 7) * m_SIZE_CONFIGURATION_c)))
  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.sas_data[6],data,m_SIZE_CONFIGURATION_c);
  }


  if(offset == g_NVM_TABLES_START_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.nvm_status,data,2);
  }

  if(offset == g_NEIGHBOR_TABLE_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.Neighbor_Table,data,sizeof(app_info->nvm_storage.Neighbor_Table));
  }


//#if ( g_NWK_MESH_ROUTING_d == 1 )
#if (defined(ZIGBEE_ROUTER) || (ZIGBEE_COORDINATOR))
  if(offset == g_ROUTE_TABLE_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.Route_Table,data,sizeof(app_info->nvm_storage.Route_Table));
  }
#endif 

  if(offset == g_SECURITY_MATERIAL_DESCRIPTOR_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.Security_Material_Descriptor,data,sizeof(app_info->nvm_storage.Security_Material_Descriptor));
  }


  if(offset == g_PERSISTENT_NIB_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.Persistent_NIB,data,sizeof(app_info->nvm_storage.Persistent_NIB));
  }


  if(offset == g_ZDO_INFORMATION_BASE_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.ZDO_Information_Base,data,sizeof(app_info->nvm_storage.ZDO_Information_Base));
  }


//#if ( g_NWK_MANY_TO_ONE_ROUTING_d == 1 )
//#if (g_NWK_ROUTE_RECORD_TABLE_d == 1)
  if(offset == g_ROUTE_RECORD_TABLE_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.route_record_table,data,sizeof(app_info->nvm_storage.route_record_table));
  }
//#endif

  if(offset == g_LINK_KEY_COUNTER_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.LinkKeyFrameCounterTable,data,sizeof(app_info->nvm_storage.LinkKeyFrameCounterTable));
  }


  if(offset == g_LINK_KEY_TABLE_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.LinkKeyDeviceKeyPairTable,data,sizeof(app_info->nvm_storage.LinkKeyDeviceKeyPairTable));
  }


  if(offset == g_BINDING_TABLE_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.src_bind_enrty,data,sizeof(app_info->nvm_storage.src_bind_enrty));
  }


//#if ( g_APS_GROUP_DATA_RX_d == 1 )
//#ifdef ZIGBEE_END_DEVICE 
  if(offset == g_GROUP_TABLE_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.Group_Table,data,sizeof(app_info->nvm_storage.Group_Table));
  }
//#endif       


  if(offset == g_ADDRESS_MAP_TABLE_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.Address_Map_Table,data,sizeof(app_info->nvm_storage.Address_Map_Table));
  }


//#if ( g_APS_LAYER_SEC_PROCESSING_d == 1 )
//#if (( g_TRUST_CENTRE_d == 1 ) && ( g_USE_HASH_KEY == 0 ) )
//#if ( g_APP_LINK_KEY_TABLE_app_info->nvm_storage_d == ENABLE )
#ifdef ZIGBEE_COORDINATOR
  if(offset == g_APP_LINK_KEY_TABLE_OFFSET_c)
  {
    memUtils_memCopy((uint8_t*)app_info->nvm_storage.LinkKeyTable,data,sizeof(LinkKeyTable_t));
  }


  if(offset == (g_APP_LINK_KEY_TABLE_OFFSET_c + (1 * sizeof(LinkKeyTable_t))))
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.LinkKeyTable[1],data,sizeof(LinkKeyTable_t));
  }


  if(offset == (g_APP_LINK_KEY_TABLE_OFFSET_c + (2 * sizeof(LinkKeyTable_t))))
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.LinkKeyTable[2],data,sizeof(LinkKeyTable_t));
  }


  if(offset == (g_APP_LINK_KEY_TABLE_OFFSET_c + (3 * sizeof(LinkKeyTable_t))))
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.LinkKeyTable[3],data,sizeof(LinkKeyTable_t));
  }


  if(offset == (g_APP_LINK_KEY_TABLE_OFFSET_c + (4 * sizeof(LinkKeyTable_t))))
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.LinkKeyTable[4],data,sizeof(LinkKeyTable_t));
  }


  if(offset == (g_APP_LINK_KEY_TABLE_OFFSET_c + (5 * sizeof(LinkKeyTable_t))))
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.LinkKeyTable[5],data,sizeof(LinkKeyTable_t));
  }

  if(offset == (g_APP_LINK_KEY_TABLE_OFFSET_c + (6 * sizeof(LinkKeyTable_t))))
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.LinkKeyTable[6],data,sizeof(LinkKeyTable_t));
  }

  if(offset == (g_APP_LINK_KEY_TABLE_OFFSET_c + (7 * sizeof(LinkKeyTable_t))))
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.LinkKeyTable[7],data,sizeof(LinkKeyTable_t));
  }

  if(offset == (g_APP_LINK_KEY_TABLE_OFFSET_c + (8 * sizeof(LinkKeyTable_t))))
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.LinkKeyTable[8],data,sizeof(LinkKeyTable_t));
  }

  if(offset == (g_APP_LINK_KEY_TABLE_OFFSET_c + (9 * sizeof(LinkKeyTable_t))))
  {
    memUtils_memCopy((uint8_t*)&app_info->nvm_storage.LinkKeyTable[9],data,sizeof(LinkKeyTable_t));
  }
#endif      
  rsi_zigb_app_info.state = FSM_ZB_MATCH_DESC_RESP;

}
/*===========================================================================
 *
 * @fn          uint8_t rsi_zigb_channel_ext(uint32_t channel_Mask)
 *                                                  
 * @brief       channel extract utility 
 * @param[in]   channel Mask
 * @return      Channel number
 * @section description
 * This API is used to extract channel from channel mask
 *
 * ===========================================================================*/
uint8_t rsi_zigb_channel_ext(uint32_t channel_Mask)
{
  uint8_t i;
  for(i=0 ; i<16 ; i++) {
    if((channel_Mask >> i) & (0x00000800))
      return (i+11);
  }
  return (11);
}

uint32_t rsi_zigb_GetEvents()
{
	return Event_Bitmap;
//	return 0;
}
/*===========================================================================
 *
 * @fn          int main()
 *                                                  
 * @brief       main function to trigger the api test functionality. 
 * @param[in]   none
 * @return      status - 0 :success
 *                     - -1:failure
 * @section description
 * This API is used to extract channel from channel mask
 *
 * ===========================================================================*/


/*-----------------------------------------------------------------------------
 * Interrupt Service Routines
 *----------------------------------------------------------------------------*/

/* None */

/*-----------------------------------------------------------------------------
 * End Of File
 *----------------------------------------------------------------------------*/
