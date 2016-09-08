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
#include "rsi_driver.h"
#include "rsi_framework.h"
/* returns an pointer to 64-bit extended address of the self device */

#define ZIGB_GLOBAL_BUFF_LEN   8000
#define TRIGGER_HA_SAMPLE_TOGGLE 3000000 
#define g_ON_OFF_SWITCH_TIMER_c 2000000
#define DATA_INDICATION_EVENT 0x1
/*-----------------------------------------------------------------------------
 * Global Constants
 *----------------------------------------------------------------------------*/
state_machine_t stateMachine;
lightDeviceInfo_t LightDeviceInfo;
lightDeviceInfo_t LightDeviceInfo_temp;
Simple_Descriptor_t Enddevice_simple_desc;
APSDE_Data_Indication_t DataIndication;
uint8_t on_off_data;
uint32_t toggle_cnt=1;
static uint32_t Event_Bitmap;
uint8_t zigb_global_buf[ZIGB_GLOBAL_BUFF_LEN];
extern queue_t ZDP_APP_Queue;
extern rsi_zigb_app_info_t rsi_zigb_app_info;
extern const Simple_Descriptor_t On_Off_Switch_Simple_Desc;

/* Macro that holds the baud rate of UART */
/*-----------------------------------------------------------------------------
 * Public Memory declarations
 *******************************************************************************/
void HA_SampleApp_StateMachine(void);
void HA_SampleApp_State_Timer(uint8_t timer_id);
void rsi_check_desc_resp(uint8_t timer_id);

void send_toggle(void);
void poll_for_data(void);
void rsi_zigb_event_init(void);
void rsi_zigb_SetEvent(uint32_t EventId);
void rsi_zigb_ClearEvent(uint32_t EventId);

uint32_t rsi_zigb_GetEvents();
/*-----------------------------------------------------------------------------
 * Private Memory declarations
 *----------------------------------------------------------------------------*/
static uint8_t App_Send_Data( uint8_t direction, uint8_t commandType,
				uint8_t commandId, uint16_t cluster, uint8_t dataLength,uint8_t* payloadPointer );

/*******************************************************************************
 * Public Functions
 *******************************************************************************/
int App_main ( void )
{
  int32_t status = 0;
  uint8_t device_type = 2;
  uint8_t   ieee_addr[8]={0x0};
  uint8_t   i=0;
  uint16_t nwk_addr_of_interest = 0x0000;
  uint16_t dest_addr = g_BROADCAST_ADDRESS_c;
  uint16_t* lightAddress;
  lightDeviceInfo_t lightDevInfo;
  rsi_zigb_app_info_t   *app_info = &rsi_zigb_app_info;
  rsi_zigb_cb_t *app_cb_ptr = rsi_driver_cb->zigb_cb;

  //! Driver initialization
  status = rsi_driver_init(zigb_global_buf, ZIGB_GLOBAL_BUFF_LEN);
  if((status < 0) || (status > ZIGB_GLOBAL_BUFF_LEN))
  {
    RSI_DPRINT(RSI_PL1,"\n driver init failed");
    return status;
  }

  //! RS9113 intialisation 
  status = rsi_device_init(RSI_LOAD_IMAGE_I_FW);
  if(status != RSI_SUCCESS)
  {
    RSI_DPRINT(RSI_PL1,"\n device init failed");
    return status;
  }

  //! WC initialization
  status = rsi_wireless_init(0, 3);
  if(status != RSI_SUCCESS)
  {
    return status;
  }
  TMR_Init();
  buffMgmt_init();
  //App_Init();
  rsi_zigb_register_callbacks (
      rsi_zigb_app_scan_complete_handler,
      rsi_zigb_app_energy_scan_result_handler,             
      rsi_zigb_app_network_found_handler,
      rsi_zigb_app_stack_status_handler,
      rsi_zigb_app_incoming_many_to_one_route_req_handler,
      rsi_zigb_app_handle_data_indication,
      rsi_zigb_app_handle_data_confirmation,
      rsi_zigb_app_child_join_handler,
#ifdef ZB_PROFILE    
      rsi_zigb_app_interpan_data_indication,
      rsi_zigb_app_interpan_data_confirmation
#endif
	   ,rsi_zigb_nvm_backup_handler
      );
  RSI_DPRINT(RSI_PL1,"\n Sending init stack\n");
  //! zigb stack init 
  status = rsi_zigb_init_stack();


  RSI_DPRINT(RSI_PL1,"\n Sending reset stack\n");
  //! zigb stack reset 
  status = rsi_zigb_reset_stack();
  //! zigb init scan 
  status = rsi_zigb_initiate_scan(g_MAC_ACTIVE_SCAN_TYPE_c,
      g_CHANNEL_MASK_c, g_SCAN_DURATION_c);
  sleep(3);
  if(app_info->state != FSM_SCAN_COMPLETE)
  {
    RSI_DPRINT(RSI_PL1,"\n Could Not Find Network");
    return -1;
  }
  RSI_DPRINT(RSI_PL1,"\nscan_status:0x%x", status);

  //! zigb join request 
  printf( "rsi_zigb_join_network:");
  printf("\n ## channel: %d  ",rsi_zigb_app_info.nwkinfo.channel);
  printf("\n ## Extended panid");
  for(i=0;i<8;i++)
  {
    printf(" 0x%x",rsi_zigb_app_info.nwkinfo.extendedPanId[i]);
  }
  status =rsi_zigb_join_network(device_type,
      rsi_zigb_app_info.nwkinfo.channel, 0x0f, rsi_zigb_app_info.nwkinfo.extendedPanId);

  if(app_info->state != FSM_JOIN_COMPLETE)
  {
    RSI_DPRINT(RSI_PL1,"\n Could Not Join the Network");
    exit(0);
    return -1;
  }
  RSI_DPRINT(RSI_PL1,"\njoin_status:0x%x", status);

  RSI_DPRINT(RSI_PL1,"\nrsi_zigb_set_simple_descriptor:");
  status = rsi_zigb_set_simple_descriptor(g_ON_OFF_SWITCH_ENDPOINT_c, 
      &On_Off_Switch_Simple_Desc);

  //! send match descriptor request
  status = rsi_zigb_send_match_descriptors_request(nwk_addr_of_interest,
      On_Off_Switch_Simple_Desc.app_profile_id,
      (uint8_t *)On_Off_Switch_Simple_Desc.p_incluster_list,
      On_Off_Switch_Simple_Desc.incluster_count,
      (uint8_t *)On_Off_Switch_Simple_Desc.p_outcluster_list,
      On_Off_Switch_Simple_Desc.outcluster_count,
      g_TRUE_c,
      dest_addr);

  //! poll for match descriptor response
  rsi_zigb_end_device_poll_for_data();
  //! start timer to poll until match descriptor resp is received
  tmr_startRelativeTimer(g_HA_SA_POLL_TIMER, TRIGGER_HA_SAMPLE_TOGGLE, poll_for_data);
  main_loop();

  //! Update light addr and EP on receiving match descriptor response 
  lightAddress =  (uint16_t *)&app_info->zb_resp_info.matchDescResp[2];
  LightDeviceInfo.shortaddress = *lightAddress;    
  LightDeviceInfo.endpoint = app_info->zb_resp_info.matchDescResp[5];
  RSI_DPRINT(RSI_PL1,"\n Light addr: 0x%x \n Light EP: 0x%x", LightDeviceInfo.shortaddress, LightDeviceInfo.endpoint);

  RSI_DPRINT(RSI_PL1,"\n Send toggle");

  //! update state at sending toggle 
  rsi_zigb_app_info.state = FSM_ZB_SEND_DATA;
  App_Send_Data(g_Client_To_Server_c, g_Cluster_Specific_Cmd_c,g_Toggle_Command_c,g_ON_OFF_CLUSTER_c,0x00,g_NULL_c);

  //! start timer to send toggle periodically
  tmr_startRelativeTimer(g_HA_SAMPLE_APP_TIMER_c, TRIGGER_HA_SAMPLE_TOGGLE, send_toggle);
  //! Application main loop
  main_loop();

  RSI_DPRINT(RSI_PL1,"\n End of App");
  return 0;
}

void poll_for_data(void)
{

  if(rsi_zigb_app_info.state != FSM_ZB_MATCH_DESC_RESP)
    {
      rsi_zigb_end_device_poll_for_data();
      tmr_startRelativeTimer(g_HA_SA_POLL_TIMER, TRIGGER_HA_SAMPLE_TOGGLE, poll_for_data);
    }
  else{
    tmr_stopTimer(g_HA_SA_POLL_TIMER);
  }

}

void send_toggle(void)
{
    
    if(rsi_zigb_app_info.state == FSM_ZB_DATA_CONFRM)
    {
      rsi_zigb_app_info.state = FSM_ZB_SEND_DATA;
      RSI_DPRINT(RSI_PL1,"\n Send toggle %d",++toggle_cnt);
      
      App_Send_Data(g_Client_To_Server_c, g_Cluster_Specific_Cmd_c,g_Toggle_Command_c,g_ON_OFF_CLUSTER_c,0x00,g_NULL_c);
    
    }
    tmr_startRelativeTimer(g_HA_SAMPLE_APP_TIMER_c, TRIGGER_HA_SAMPLE_TOGGLE, send_toggle);
}

void HA_SampleApp_Init()
{

		LightDeviceInfo.endpoint = 0xFF;
		LightDeviceInfo.shortaddress = 0xFFFF;
}


void intimate_read_response_data_to_app(uint8_t data)
{
		on_off_data = data;
}

void HA_SampleApp_BcastTimer(uint8_t timer_id)
{
}

void rsi_zigb_app_scan_complete_handler ( uint32_t channel, uint8_t status );
void App_init()
{
}
//*****************************************************************************
// Send_Event_To_Application
// *****************************************************************************


static uint8_t App_Send_Data( uint8_t direction, uint8_t commandType,
				uint8_t commandId, uint16_t cluster, uint8_t dataLength,uint8_t* payloadPointer )
{
		uint8_t status;
		Address DestAddress;
		ZigBeeAPSDEDataRequest_t *pAPSDERequest;
		App_ZCL_Request_t* pZcl_hdr;
		uint8_t *pAsdu;
		uint8_t bufferIndex;
		/*+1 is added for Command id*/
		uint8_t ZCLHeaderLength = ((sizeof(App_ZCL_Request_t) - sizeof(ZCL_Command_t)) + 1);

		if ((bufferIndex = buffMgmt_allocateBuffer(g_LARGE_BUFFER_c)) != NOBUFFER) {
				pAPSDERequest = (ZigBeeAPSDEDataRequest_t*)buffMgmt_getBufferPointer(bufferIndex);

				DestAddress.short_address = LightDeviceInfo.shortaddress;
				pAPSDERequest->ProfileId = On_Off_Switch_Simple_Desc.app_profile_id;
				pAPSDERequest->DestEndpoint = LightDeviceInfo.endpoint;
				pAPSDERequest->ClusterId = cluster;
				pAPSDERequest->AsduLength = dataLength;
				pAPSDERequest->SrcEndpoint = g_ON_OFF_SWITCH_ENDPOINT_c;
				pAPSDERequest->TxOptions = g_APS_Tx_Opt_Ack_Req_c | g_APS_Tx_Opt_Use_NWK_Key_c;
				pAPSDERequest->Radius = DEFAULT_RADIUS;

				pZcl_hdr = (App_ZCL_Request_t*)pAPSDERequest->aPayload;
				pZcl_hdr->command_type = commandType;
				pZcl_hdr->disable_default_response = g_Disable_Default_Response_c;
				pZcl_hdr->manufacture_specific = g_FALSE_c;
				pZcl_hdr->ZCL_command_received.command_identifier = commandId;
				pAsdu = pAPSDERequest->aPayload + ZCLHeaderLength;


				memUtils_memCopy(
								pAsdu,
								payloadPointer,
								dataLength);

				pAPSDERequest->AsduLength =  ZCL_Create_Command ( direction,
								pAPSDERequest->aPayload,
								(App_ZCL_Request_t*)&pAPSDERequest->aPayload,
								dataLength + ZCLHeaderLength ,
								App_GetTransSeqNumber());
      

				status = rsi_zigb_send_unicast_data(ZigBee_Outgoing_Direct,
								DestAddress  , pAPSDERequest);

				buffMgmt_freeBuffer(bufferIndex, 225);

				return status;
		}
    RSI_DPRINT(RSI_PL1,"\n NO BUFFER \n");
		return NOBUFFER;
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
  if(!rsi_zigb_app_info.stack_status)
  {
    rsi_zigb_app_info.state = FSM_JOIN_COMPLETE; 
  }
#ifdef ZB_DEBUG  
  RSI_DPRINT(RSI_PL1,"\n Stack Status = %x \n", *statusInfo);
#endif  
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
  int i = 0;
  rsi_zigb_app_info_t *app_info = &rsi_zigb_app_info;
  RSI_DPRINT(RSI_PL1,"\nData Indication");
#if ZB_DEBUG
  RSI_DPRINT(RSI_PL1,"\nData Indication");
  RSI_DPRINT(RSI_PL1,"\n pDataIndication->cluster_id: 0x%x",pDataIndication->cluster_id);
  RSI_DPRINT(RSI_PL1,"\n status: 0x%x",pDataIndication->a_asdu[1]);
  RSI_DPRINT(RSI_PL1,"\n Source EP: 0x%x",pDataIndication->src_endpoint);
  RSI_DPRINT(RSI_PL1,"\n Source Addr: 0x%x",pDataIndication->src_address);
  RSI_DPRINT(RSI_PL1,"\n profile_id: 0x%x",pDataIndication->profile_id);
  RSI_DPRINT(RSI_PL1,"\n cluster_id;: 0x%x\n",pDataIndication->cluster_id);
#endif
  if( pDataIndication->cluster_id == 0x8006)//0x8006: Match decs response
  {
    if(pDataIndication->a_asdu[1] == 0x00)
    {
      if(rsi_zigb_app_info.state != FSM_ZB_MATCH_DESC_RESP)
      {
        rsi_zigb_mcpy( pDataIndication->a_asdu, 
            app_info->zb_resp_info.matchDescResp,
            pDataIndication->asdulength);
        app_info->status_var.matchDescRspStatus = 0x00;
        rsi_zigb_app_info.state = FSM_ZB_MATCH_DESC_RESP;
      }
    }
  }
  if(rsi_zigb_app_info.state > FSM_ZB_MATCH_DESC_RESP)
  {
    rsi_zigb_mcpy( pDataIndication, 
        &DataIndication,
        sizeof(APSDE_Data_Indication_t));
    rsi_zigb_SetEvent(DATA_INDICATION_EVENT);
  }
  //Handle_ZCL_Message(9,pDataIndication);
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
void rsi_zigb_app_handle_data_confirmation (APSDE_Data_Confirmation_t *pDataConfirmation)
{
  RSI_DPRINT(RSI_PL1,"\n Received confirmation");
  if((g_Aps_Success_c == pDataConfirmation->status) && (g_ON_OFF_SWITCH_ENDPOINT_c == pDataConfirmation->src_endpoint))
  {
    if(rsi_zigb_app_info.state == FSM_ZB_SEND_DATA)
    {
      rsi_zigb_app_info.state = FSM_ZB_DATA_CONFRM;
    }
  }
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

void rsi_zigb_app_interpan_data_indication( uint8_t bufferid, APSDE_InterPan_Data_Indication_t *pApsdeDataInd )
{
#if (g_APP_PROFILE_ID_c==0xc05e)
#if ( g_INTERPAN_DATA_ENABLE_d== 1 )
    App_InterPanDataindication(bufferid,pApsdeDataInd);
#endif
#endif
}

void rsi_zigb_app_interpan_data_confirmation( uint8_t bufferid, APSDE_InterPan_Data_Confirmation_t *pInterPanDataConf )
{
#if (g_APP_PROFILE_ID_c==0xc05e)
#if ( g_INTERPAN_DATA_ENABLE_d== 1 )
  App_InterPanDataConfirmation(bufferid,pInterPanDataConf);
#endif
#endif
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

void main_loop(void)
{
    uint32 event_map = 0; 
  while(1)
  {
    event_map = rsi_zigb_GetEvents(); 
    ////////////////////////
    //! Application code ///
    ////////////////////////
    switch(event_map)
    {
      case DATA_INDICATION_EVENT:
        Handle_ZCL_Message(9,&DataIndication);
        rsi_zigb_ClearEvent(DATA_INDICATION_EVENT);
        break;

      default:
        break;
    }
    //! event loop 
    rsi_non_os_event_loop();

   if(rsi_zigb_app_info.state == FSM_ZB_MATCH_DESC_RESP)
     break;
  }
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
int main()
{
  int32_t status=0;
  //! Call TCP client application
  status = App_main();
  RSI_DPRINT(RSI_PL1,"\n rsi_zigb_api_test:0x%x",status);
  return 0;
}

void rsi_zigb_event_init(void)
{
	Event_Bitmap = 0;
}

void rsi_zigb_SetEvent(uint32_t EventId)
{
	Event_Bitmap |= EventId;
}

void rsi_zigb_ClearEvent(uint32_t EventId)
{
	Event_Bitmap &= ~(EventId);
}

uint32_t rsi_zigb_GetEvents()
{
	return Event_Bitmap;
//	return 0;
}

/*-----------------------------------------------------------------------------
 * Interrupt Service Routines
 *----------------------------------------------------------------------------*/

/* None */

/*-----------------------------------------------------------------------------
 * End Of File
 *----------------------------------------------------------------------------*/
