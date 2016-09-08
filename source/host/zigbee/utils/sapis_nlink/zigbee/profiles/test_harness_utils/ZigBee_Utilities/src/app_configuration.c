/*******************************************************************************
 * application_configuration.c
 ********************************************************************************
 * Program Description:
 *     This file contains the stack application configurable parameters.
 *
 ********************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "ZigBeeDataTypes.h"
#include "memory_utility.h"
#include "stack_common.h"
#include "stack_functionality.h"
#include "app_configuration_default.h"
#include "stack_util.h"
#include "nlme_interface.h"
#include "zdo_interface.h"
#include "zdp_interface.h"
#include "stack_vars.h"
#include "apsde_interface.h"
#if (g_APP_PROFILE_ID_c==0x0109)
#include "ZCL_Functionality.h"
#include "ZCL_Interface.h"
#include "AppCBKEUtilities.h"
#endif



#if (g_APP_PROFILE_ID_c== 0xc05e)
#include "ZLL_defines.h"
#endif /*g_APP_PROFILE_ID_c== 0xc05e*/

/*******************************************************************************
 * Global Constants
 *******************************************************************************/
 /* Hex values for 1000 bytes i.e 1 KBytes */
#define m_ONE_KB_c					                    0x400

/* starting location of NVM indicates the staus of the NVM, it takes 2 bytes  */
#define g_NVM_SAVE_STATUS_BYTE_SIZE_c              		0x02




/******************************************************************************
 Neigbhor table size = size for 1 entry of Neighbor Table
 * num of max entries
 ******************************************************************************/
/*
#define g_NEIGHBOR_TABLE_SIZE_c \
            ((( sizeof( NeighborTable_t ) * g_MAX_NEIGHBOR_TABLE_ENTRIES_c ) & 1)?\
              ( (sizeof( NeighborTable_t ) * g_MAX_NEIGHBOR_TABLE_ENTRIES_c) + 1):\
               ( sizeof( NeighborTable_t ) * g_MAX_NEIGHBOR_TABLE_ENTRIES_c ))
*/
#define g_NEIGHBOR_TABLE_SIZE_c \
           ( sizeof( NeighborTable_t ) * g_MAX_NEIGHBOR_TABLE_ENTRIES_c )

/******************************************************************************
 security descriptor size = size for 1 entry of route table
 * num of max entries
 ******************************************************************************/
/*
#define g_SECURITY_MATERIAL_DESCRIPTOR_SIZE_c \
            ((( sizeof( Security_Material_Descriptor_t ) * g_MAX_NWK_KEY_c )&1)?\
              (( sizeof( Security_Material_Descriptor_t ) * g_MAX_NWK_KEY_c )+1):\
               ( sizeof( Security_Material_Descriptor_t ) * g_MAX_NWK_KEY_c ))
*/
#define g_SECURITY_MATERIAL_DESCRIPTOR_SIZE_c \
               ( sizeof( Security_Material_Descriptor_t ) * g_MAX_NWK_KEY_c )

/******************************************************************************
  Persistent NIB Memory Size
 ******************************************************************************/
/*
#define g_PERSISTENT_NIB_SIZE_c  ((sizeof( Persistent_NIB_t )&1)?\
                                  (sizeof( Persistent_NIB_t )+1):\
                                   sizeof( Persistent_NIB_t ))
 */
#define g_PERSISTENT_NIB_SIZE_c     (sizeof( Persistent_NIB_t )  )

/******************************************************************************
  APS Address map Table Size = size of 1 entry of address map table * num of
 max entries
 ******************************************************************************/

#define g_ADDRESS_MAP_TABLE_SIZE_c \
             (sizeof(APSME_Address_Map_Table_t ) * g_MAX_ADDR_MAP_ENTRIES_c)

/******************************************************************************
 ZDO Information Base Size
 ******************************************************************************/
#define g_ZDO_INFORMATION_BASE_SIZE_c ((sizeof( ZDO_Information_Base_t )&1)?\
                                        (sizeof( ZDO_Information_Base_t ) + 1):sizeof( ZDO_Information_Base_t ))

/******************************************************************************
 Group Table Size = size of 1 entry of group table * num of max entries
 ******************************************************************************/
#if ( g_APS_GROUP_DATA_RX_d == ENABLE )
    #define g_GROUP_TABLE_SIZE_c \
            ( sizeof( Group_Table_t ) * g_MAX_GROUP_TABLE_ENTRIES_c )
#else
    #define g_GROUP_TABLE_SIZE_c        (g_NULL_c)
#endif    /* g_APS_GROUP_DATA_RX_d*/


/******************************************************************************
 Route table size = size for 1 entry * num of max entries
 ******************************************************************************/
#if( g_NWK_MESH_ROUTING_d == ENABLE )
/*
    #define g_ROUTE_TABLE_SIZE_c  \
            ((( sizeof( Route_Table_t ) * g_MAX_ROUTE_TABLE_ENTRIES_c )&1)?\
              (( sizeof( Route_Table_t ) * g_MAX_ROUTE_TABLE_ENTRIES_c )+1):\
                ( sizeof( Route_Table_t ) * g_MAX_ROUTE_TABLE_ENTRIES_c ))
 */
#define g_ROUTE_TABLE_SIZE_c  \
            ( sizeof( Route_Table_t ) * g_MAX_ROUTE_TABLE_ENTRIES_c )
#else
    #define g_ROUTE_TABLE_SIZE_c   ( g_NULL_c)
#endif    /* g_NWK_MESH_ROUTING_d*/

/******************************************************************************
 * Define Route Record table size for Many to One Routing
 ******************************************************************************/
#if (( g_NWK_MANY_TO_ONE_ROUTING_d == ENABLE ) && (g_NWK_SOURCE_ROUTING_d == ENABLE))

#define g_ROUTE_RECORD_TABLE_SIZE_c \
                (sizeof( Route_Record_Table_t ) * g_MAX_ROUTE_RECORD_TABLE_ENTRIES_c)
#else
    /* One Default Entry would be allocated*/
#define g_ROUTE_RECORD_TABLE_SIZE_c    sizeof( Route_Record_Table_t )
#endif     /* g_NWK_MANY_TO_ONE_ROUTING_d*/

/******************************************************************************
 * Define Binding Table Size
 ******************************************************************************/

#if ( g_APS_BINDING_CAPACITY_c == ENABLE )
    #define g_BINDING_TABLE_SIZE_c \
	  		((sizeof( Source_Binding_Table_t ) * g_MAX_BINDING_TABLE_ENTRIES_c))
#else
    #define g_BINDING_TABLE_SIZE_c        (g_NULL_c)
#endif   /* g_APS_BINDING_CAPACITY_c*/

/******************************************************************************
 * Define Link Key Table Size
 ******************************************************************************/

#if ( g_APS_LAYER_SEC_PROCESSING_d == ENABLE )
    #define g_LINK_KEY_TABLE_SIZE_c \
					 (sizeof(LinkKeyDeviceKeyPair_t) * g_MAX_LINK_KEY_ENTRIES_c)
    #define g_LINK_KEY_COUNTER_SIZE_c \
	  				 (sizeof(LinkKeyFrameCounter_t) * g_MAX_LINK_KEY_ENTRIES_c)
#if ( g_TRUST_CENTRE_d == ENABLE ) && ( g_USE_HASH_KEY == DISABLE )
    #define g_APP_LINK_KEY_TABLE_SIZE_c \
	  					(sizeof(LinkKeyTable_t) * g_MAX_APP_LINK_KEY_ENTRIES_c)
#else
    #define g_APP_LINK_KEY_TABLE_SIZE_c         (g_NULL_c)
#endif
#else
    #define g_LINK_KEY_TABLE_SIZE_c             (g_NULL_c)
    #define g_LINK_KEY_COUNTER_SIZE_c           (g_NULL_c)
    #define g_APP_LINK_KEY_TABLE_SIZE_c         (g_NULL_c)
#endif   /* g_APS_LAYER_SEC_PROCESSING_d*/

/******************************************************************************
 * CBKE table size
 ******************************************************************************/
#if ( g_KEY_ESTABLISHMENT_CLUSTER_ENABLE_d == ENABLE )

	#define  g_CBKE_TABLE_SIZE_c  (100)
#else
    #define  g_CBKE_TABLE_SIZE_c    (g_NULL_c)

#endif  /*g_KEY_ESTABLISHMENT_CLUSTER_ENABLE_d */

#if (g_CBKE_INFO_TABLE_NVM_STORAGE_d == DISABLE )
	#define g_RAM_CBKE_SIZE_c		( g_CBKE_TABLE_SIZE_c )
	#define g_NVM_CBKE_SIZE_c		(  g_NULL_c  )
#else

	#define g_RAM_CBKE_SIZE_c		( g_NULL_c )
	#define g_NVM_CBKE_SIZE_c		( g_CBKE_TABLE_SIZE_c  )
#endif    /*g_CBKE_INFO_TABLE_NVM_STORAGE_d */

/******************************************************************************
 * Global defintions for NVM storage
 ******************************************************************************/

#if ( g_BIND_TABLE_NVM_STORAGE_d == DISABLE )
    #define g_RAM_BIND_TBL_SIZE_c          ( g_BINDING_TABLE_SIZE_c)
    #define g_NVM_BIND_TBL_SIZE_c          ( g_NULL_c)
#else
    #define g_RAM_BIND_TBL_SIZE_c           (g_NULL_c)
    #define g_NVM_BIND_TBL_SIZE_c          ( g_BINDING_TABLE_SIZE_c)
#endif   /* ( g_BIND_TABLE_NVM_STORAGE_d == DISABLE ) */

/*****************************************************************************/
#if ( g_GROUP_TABLE_NVM_STORAGE_d == DISABLE )
    #define g_RAM_GROUP_TBL_SIZE_c      (g_GROUP_TABLE_SIZE_c)
    #define g_NVM_GROUP_TBL_SIZE_c      (g_NULL_c)
#else
    #define g_RAM_GROUP_TBL_SIZE_c      (g_NULL_c)
    #define g_NVM_GROUP_TBL_SIZE_c      (g_GROUP_TABLE_SIZE_c)
#endif   /* ( g_GROUP_TABLE_NVM_STORAGE_d == DISABLE )*/
/*****************************************************************************/

#if ( g_ADDRESS_MAP_NVM_STORAGE_d == DISABLE )
    #define g_RAM_ADDRMAP_TBL_SIZE_c    (g_ADDRESS_MAP_TABLE_SIZE_c)
    #define g_NVM_ADDRMAP_TBL_SIZE_c    (g_NULL_c )
#else
    #define g_RAM_ADDRMAP_TBL_SIZE_c    (g_NULL_c)
    #define g_NVM_ADDRMAP_TBL_SIZE_c    (g_ADDRESS_MAP_TABLE_SIZE_c)
#endif   /* ( g_ADDRESS_MAP_NVM_STORAGE_d == DISABLE ) */
/*****************************************************************************/

/*Link Key table Size*/
#define g_RAM_LINK_KEY_TBL_SIZE_c   (g_LINK_KEY_TABLE_SIZE_c)


/*****************************************************************************/

#if ( g_APP_LINK_KEY_TABLE_NVM_STORAGE_d == DISABLE )
    #define g_RAM_APP_LINK_KEY_TBL_SIZE_c   (g_APP_LINK_KEY_TABLE_SIZE_c)
    #define g_NVM_APP_LINK_KEY_TBL_SIZE_c   (g_NULL_c)
#else
    #define g_RAM_APP_LINK_KEY_TBL_SIZE_c   (g_NULL_c)
    #define g_NVM_APP_LINK_KEY_TBL_SIZE_c   (g_APP_LINK_KEY_TABLE_SIZE_c)
#endif   /* ( g_APP_LINK_KEY_TABLE_NVM_STORAGE_d == DISABLE ) */

/*****************************************************************************/
/* Location from where Neighbor Table starts in the persistent memory sector
 one */
/*****************************************************************************/

#define g_NEIGHBOR_TABLE_OFFSET_c       (g_NVM_SAVE_STATUS_BYTE_SIZE_c)

/*****************************************************************************/
/* Starting Location of Route Table in the persistent memory sector one */
/*****************************************************************************/

#define g_ROUTE_TABLE_OFFSET_c    \
  							(g_NEIGHBOR_TABLE_OFFSET_c + g_NEIGHBOR_TABLE_SIZE_c)
/*****************************************************************************/

/* Starting Location of Security material Set Table in the persistent memory
 sector one */
/*****************************************************************************/

#define g_SECURITY_MATERIAL_DESCRIPTOR_OFFSET_c\
                                 ( g_ROUTE_TABLE_OFFSET_c + g_ROUTE_TABLE_SIZE_c)

/*****************************************************************************/
/* Starting Location of Persistent Variable from NIB in the persistent memory
 sector one */
/*****************************************************************************/

#define g_PERSISTENT_NIB_OFFSET_c     ( g_SECURITY_MATERIAL_DESCRIPTOR_OFFSET_c + \
                                        g_SECURITY_MATERIAL_DESCRIPTOR_SIZE_c )

#define g_ZDO_INFORMATION_BASE_OFFSET_c (g_PERSISTENT_NIB_OFFSET_c + \
                                        g_PERSISTENT_NIB_SIZE_c )

#define g_ROUTE_RECORD_TABLE_OFFSET_c   ( g_ZDO_INFORMATION_BASE_OFFSET_c + \
                                        g_ZDO_INFORMATION_BASE_SIZE_c )

#define g_LINK_KEY_COUNTER_OFFSET_c    ( g_ROUTE_RECORD_TABLE_OFFSET_c + \
                                        g_ROUTE_RECORD_TABLE_SIZE_c )

#define g_LINK_KEY_TABLE_OFFSET_c    \
  					( g_LINK_KEY_COUNTER_OFFSET_c + g_LINK_KEY_COUNTER_SIZE_c)
/*****************************************************************************/

#if ( ZIGBEE_LIB_MEMORY_OPTIMIZATION == DISABLE )
/*****************************************************************************/
/* Starting Location of APS Binding Table in persitent memory two added to make it
   16 bit aligned */
/*****************************************************************************/

#define g_BINDING_TABLE_OFFSET_c        \
  					(g_LINK_KEY_TABLE_OFFSET_c + g_RAM_LINK_KEY_TBL_SIZE_c)

/*****************************************************************************/
/* Starting Location of Group Table in persistent memory sector one
 Checking for 16 bit alignment*/
/*****************************************************************************/
#define g_GROUP_TABLE_OFFSET_c        \
							(g_BINDING_TABLE_OFFSET_c + g_RAM_BIND_TBL_SIZE_c)

/*****************************************************************************/
/* Location from where the APS Address Map Table Starts in persistent memory
 sector two. Checking for 16 bit alignment */
/*****************************************************************************/
#define g_ADDRESS_MAP_TABLE_OFFSET_c  \
  							(g_GROUP_TABLE_OFFSET_c + g_RAM_GROUP_TBL_SIZE_c)

/*****************************************************************************/

/*****************************************************************************/
#define g_APP_LINK_KEY_TABLE_OFFSET_c \
					(  g_ADDRESS_MAP_TABLE_OFFSET_c + g_RAM_ADDRMAP_TBL_SIZE_c)


/*****************************************************************************/
 /* starting location of CBKE  entries in persistent memory  */
/*****************************************************************************/
#if ( g_TRUST_CENTRE_d == 1 )
#define g_APP_CBKE_TABLE_OFFSET_c \
                           ( g_APP_LINK_KEY_TABLE_OFFSET_c + g_APP_LINK_KEY_TABLE_SIZE_c)
#else
#define g_APP_CBKE_TABLE_OFFSET_c \
                           ( g_APP_LINK_KEY_TABLE_OFFSET_c + g_APP_LINK_KEY_TABLE_SIZE_c)

#endif    /*  g_TRUST_CENTRE_d */

#define g_STACK_TABLES_MEMORY_SIZE_c  ( g_NEIGHBOR_TABLE_SIZE_c + \
          g_ROUTE_TABLE_SIZE_c + \
          g_SECURITY_MATERIAL_DESCRIPTOR_SIZE_c + \
          g_PERSISTENT_NIB_SIZE_c + \
          g_ZDO_INFORMATION_BASE_SIZE_c + \
          g_ROUTE_RECORD_TABLE_SIZE_c + \
          g_LINK_KEY_COUNTER_SIZE_c + \
          g_RAM_LINK_KEY_TBL_SIZE_c + \
          g_RAM_BIND_TBL_SIZE_c + \
          g_RAM_GROUP_TBL_SIZE_c + \
          g_RAM_ADDRMAP_TBL_SIZE_c + \
          g_RAM_APP_LINK_KEY_TBL_SIZE_c + \
          g_RAM_CBKE_SIZE_c  )


#else
/* If NVM is Enabled  */
#define g_BINDING_TABLE_OFFSET_c       \
  					(g_LINK_KEY_TABLE_OFFSET_c + g_RAM_LINK_KEY_TBL_SIZE_c)
#define g_GROUP_TABLE_OFFSET_c        \
						(  g_BINDING_TABLE_OFFSET_c + g_NVM_BIND_TBL_SIZE_c )
#define g_ADDRESS_MAP_TABLE_OFFSET_c  \
						(  g_GROUP_TABLE_OFFSET_c + g_NVM_GROUP_TBL_SIZE_c  )
#define g_APP_LINK_KEY_TABLE_OFFSET_c \
  						( g_ADDRESS_MAP_TABLE_OFFSET_c + g_NVM_ADDRMAP_TBL_SIZE_c )

#if ( g_TRUST_CENTRE_d == 1 )
#define g_APP_CBKE_TABLE_OFFSET_c \
                           ( g_APP_LINK_KEY_TABLE_OFFSET_c + g_NVM_APP_LINK_KEY_TBL_SIZE_c)
#else
#define g_APP_CBKE_TABLE_OFFSET_c \
                           ( g_APP_LINK_KEY_TABLE_OFFSET_c + g_NVM_APP_LINK_KEY_TBL_SIZE_c)

#endif    /*  g_TRUST_CENTRE_d */


#if (g_APP_PROFILE_ID_c== 0xc05e)

#define g_APP_ZLL_NVM_PARAMS_OFFSET_c                                  \
                             (g_APP_CBKE_TABLE_OFFSET_c + g_NVM_CBKE_SIZE_c)

#endif /* #if (g_APP_PROFILE_ID_c== 0xc05e)*/

#define g_STACK_TABLES_MEMORY_SIZE_c  ( g_NEIGHBOR_TABLE_SIZE_c + \
                                        g_ROUTE_TABLE_SIZE_c + \
                                        g_SECURITY_MATERIAL_DESCRIPTOR_SIZE_c + \
                                        g_PERSISTENT_NIB_SIZE_c + \
                                        g_ZDO_INFORMATION_BASE_SIZE_c + \
                                        g_ROUTE_RECORD_TABLE_SIZE_c + \
                                        g_LINK_KEY_COUNTER_SIZE_c + \
                                        g_RAM_LINK_KEY_TBL_SIZE_c + \
                                        g_NVM_BIND_TBL_SIZE_c + \
                                        g_NVM_GROUP_TBL_SIZE_c + \
                                        g_NVM_ADDRMAP_TBL_SIZE_c + \
                                        g_NVM_APP_LINK_KEY_TBL_SIZE_c + \
                                        g_NVM_CBKE_SIZE_c  )

#endif       /*ZIGBEE_LIB_MEMORY_OPTIMIZATION */



/*****************************************************************************/
/* Defines the stack memory size  */
/*****************************************************************************/





/*******************************************************************************
 * Public Memory declarations
 *******************************************************************************/
uint8_t ga_Stack_Tables_Data[ g_STACK_TABLES_MEMORY_SIZE_c ];



#if ( g_COMMISSIONING_CAPABILITIES_d == 1 )

#define  m_NUMBER_OF_START_UP_ATTRIBUTE_SETS_c                    0x03

#else                           /* ( g_COMMISSIONING_CAPABILITIES_d == 1 ) */

#define  m_NUMBER_OF_START_UP_ATTRIBUTE_SETS_c                    0x01

#endif                          /* g_COMMISSIONING_CAPABILITIES_d == 1 */


/* this structure contains  default values of NIB attributes
that require persistent storage. */

const NWK_Info_Base g_NWK_InfoBaseDefault_c =
{
    g_NWK_SEQUENCE_NUMBER_DEF_VALUE_c           ,
    g_FOUR_TIMES_OF_BROADCAST_PASSIVE_ACK_TIME_OUT_c,
    g_MAX_BROADCAST_RETRIES_c                   ,
    //g_NWK_MAX_CHILDREN_c                        ,
    g_NWK_MAX_DEPTH_c                           ,
   // g_NWK_MAX_ROUTER_c                          ,
    g_BROADCAST_DELIVERY_TIME_c                 ,
    g_NWK_REPORT_CONSTANT_COST_DEF_VALUE_c      ,
    g_NWK_ROUTE_DISCOVERY_RETRIES_DEF_VALUE_c   ,
    g_NWK_SYM_LINK_c                            ,
    g_NWK_CAPABILITY_INFORMATION_DEF_VALUE_c    ,
    g_NWK_USE_TREE_ADDRESS_ALLOC_DEF_VALUE_c    ,
    g_USE_TREE_ROUTING_c                        ,
    g_NWK_TRANSACTION_PERSISTENCE_TIME_c        ,
    g_STACK_PROFILE_c                           ,
    g_NWK_NBT_CURRENT_INDEX_DEF_VALUE_c         ,
    g_NWK_RT_CURRENT_INDEX_DEF_VALUE_c          ,
    g_NWK_SEC_MAT_DESCRIP_INDEX_DEF_VALUE_C     ,
    g_NWK_LOGICAL_CHANNEL_DEF_VALUE_c           ,
    g_NWK_ENERGY_THRESHOLD_DEF_VALUE_c          ,
    g_NWK_IS_CONCENTRATOR_DEF_VALUE_c           ,
    g_NWK_CONCENTRATOR_RADIUS_DEF_VALUE_c       ,
    g_NWK_CONCENTRATOR_DISCOVERYTIME_DEF_VALUE_c,
    g_NWK_UNIQUE_ADDRESS_c
#ifdef APPLY_R_20_CHANGE
    ,g_NWK_LEAVE_REQUEST_ALLOWED_c
#endif
};


/* memory for network information base attributes that does not need to be
stored in persistent storage */
NWK_Info_Base g_NWK_Info_Base =
{
    g_NWK_SEQUENCE_NUMBER_DEF_VALUE_c           ,
    g_FOUR_TIMES_OF_BROADCAST_PASSIVE_ACK_TIME_OUT_c,
    g_MAX_BROADCAST_RETRIES_c                   ,
    //g_NWK_MAX_CHILDREN_c                        ,
    g_NWK_MAX_DEPTH_c                           ,
   // g_NWK_MAX_ROUTER_c                          ,
    g_BROADCAST_DELIVERY_TIME_c                 ,
    g_NWK_REPORT_CONSTANT_COST_DEF_VALUE_c      ,
    g_NWK_ROUTE_DISCOVERY_RETRIES_DEF_VALUE_c   ,
    g_NWK_SYM_LINK_c                            ,
    g_NWK_CAPABILITY_INFORMATION_DEF_VALUE_c    ,
    g_NWK_USE_TREE_ADDRESS_ALLOC_DEF_VALUE_c    ,
    g_USE_TREE_ROUTING_c                        ,
    g_NWK_TRANSACTION_PERSISTENCE_TIME_c        ,
    g_STACK_PROFILE_c                           ,
    g_NWK_NBT_CURRENT_INDEX_DEF_VALUE_c         ,
    g_NWK_RT_CURRENT_INDEX_DEF_VALUE_c          ,
    g_NWK_SEC_MAT_DESCRIP_INDEX_DEF_VALUE_C     ,
    g_NWK_LOGICAL_CHANNEL_DEF_VALUE_c           ,
    g_NWK_ENERGY_THRESHOLD_DEF_VALUE_c          ,
    g_NWK_IS_CONCENTRATOR_VALUE_c           ,
    g_NWK_CONCENTRATOR_RADIUS_VALUE_c       ,
    g_NWK_CONCENTRATOR_DISCOVERYTIME_DEF_VALUE_c,
    g_NWK_UNIQUE_ADDRESS_c
#ifdef APPLY_R_20_CHANGE
        ,
    g_NWK_LEAVE_REQUEST_ALLOWED_c
#endif
};

/* Default value of the ZDO configuration table */
extern ZDO_Configuration_Table_t g_Table_Default;
#if 0
const ZDO_Configuration_Table_t g_Table_Default =
{
    g_PERMIT_JOIN_DURATION_c,
    g_NWK_SECURE_ALL_FRAMES_c,
    g_NWK_ALT_PROTOCOL_VERSION_c,
    g_FORMATION_ATTEMPTS_c,
    g_SCAN_DURATION_c,
    g_JOIN_ATTEMPTS_c,
    g_PRECONFIGURED_KEY_c,
    g_NO_OF_DEVICES_JOINED_BEFORE_NVM_SAVE_c,
    g_NO_OF_DATA_TXNS_BFORE_NVM_SAVE_c,
    g_TRUST_CENTER_SHORT_ADDRESS_c,
    g_AUTOMATIC_POLL_ALLOWED_c,
    g_AUTHENTICATION_POLL_RATE_c,
    g_SWITCH_KEY_TIMEOUT_c,
    g_NWK_SECURITY_LEVEL_c,
    g_APS_ACK_POLL_TIME_OUT_c,
    {
    	g_FREQ_BAND_c | g_APS_DEVICE_VERSION_FLAGS_c,
		g_MAC_CAP_FLAGS_c,
		g_MANUFACTURER_CODE_c,
		g_MAX_BUFFER_SIZE_c,
		g_MAX_INCOMING_TRANSFER_SIZE_c,
		g_MAX_OUTGOING_TRANSFER_SIZE_c,
		g_DESCRIPTOR_CAPABILITY_FIELD_c
    },
    g_CURRENT_PWR_MODE_AVAIL_PWR_SRCS,
    g_CURRENT_PWR_SRC_AND_PWR_SRC_LEVEL
};
#endif
/* 1 Default values of the Startup Attribute Set */
Startup_Attribute_Set_t Startup_Attribute_Set_Default =
{
    g_EXTENDED_PAN_ID_c,
    g_CHANNEL_MASK_c,
    g_PROTOCOL_VERSION_c,
    g_STACK_PROFILE_c,
    g_STARTUP_CONTROL_c,
    g_TRUST_CENTER_ADDRESS_c,
    g_NETWORK_KEY_c,
    g_USE_INSECURE_JOIN_c,
    g_NETWORK_KEY_DEF_SEQUENCE_NUMBER_c,
    g_NETWORK_KEY_TYPE_c,
    g_NETWORK_MANAGER_ADDRESS_c,
    g_SCAN_ATTEMPTS_c,
    g_TIME_BETWEEN_SCANS_c,
    g_REJOIN_INTERVAL_c,
    g_MAX_REJOIN_INTERVAL_c,
    g_POLL_RATE_c,
    g_PARENT_RETRY_THRESHOLD_c,
    g_SECURITY_TIMEOUT_PERIOD_c,
    g_APS_ACK_WAIT_DURATION_c,
    g_SHORT_ADDRESS_c,
    g_PANID_c,
    g_TC_MASTER_KEY_c,
    g_PRECONFG_LINK_KEY_c,
    g_NWK_IS_CONCENTRATOR_VALUE_c,                      /* defined in stack and app */
    g_NWK_CONCENTRATOR_RADIUS_VALUE_c,                   /* defined in stack and app */
    g_NWK_CONCENTRATOR_DISCOVERYTIME_DEF_VALUE_c,         /* defined in stack and app */
    g_DEVICE_ANNCE_FLAG_c,
    g_END_DEVICE_TIMEOUT_c
};


/* ZDO Information Base */
const ZDO_Information_Base_t Default_ZDO_Information_Base =
{
   g_LOGICAL_TYPE_COMPLEX_USER_DESCRIPTOR_AVAILABLE_c,
   g_SERVER_MASK_c,
   g_EXTENDED_ADDRESS_c,
   {'R','E','D','P','I','N','E','\0'},
   0x00
#ifdef ZCP_15
   ,
   0x0a,
   {0x00},
   {0x01,0x01,0x02,0x03},
   {0x02,0x04,'B','B','B','B'}, /* Manufacturer name */
   {0x03,0x04,'C','C','C','C'}, /* Model Name */
   {0x04,0x04,'1','2','3','4'}, /* Serial Number */
   {0x05,0x04,'5','6','7','8'}, /* Device URL */
   {0x06,0x04,'D','D','D','D'}, /* Icon */
   {0x07,0x04,'M','N','O','P'}, /* Icon URL */
   {0x00}
#endif // #ifdef ZCP_15
};



/*  Zigbee Stack Profile Settings
 ------------------------------ */
//
///*
// Max_Depth - defines the maximum depth for a given network
// */
uint8_t g_Max_Depth_c = g_NWK_MAX_DEPTH_c;
uint8_t g_NWK_Secure_All_Frames_c = g_NWK_SECURE_ALL_FRAMES_c;
uint8_t g_Nwk_Use_Multicast_c = g_NWK_USE_MULTICAST_c;
uint8_t g_Nwk_Security_Level_c	= g_NWK_SECURITY_LEVEL_c;
uint8_t g_Mac_Cap_flags = g_MAC_CAP_FLAGS_c;
//TBD*** to be removed
//uint16_t g_Low_Ram_Concentrator_c = ZIGBEE_LOW_RAM_CONCENTRATOR;
//uint16_t g_High_Ram_Concentrator_c = ZIGBEE_HIGH_RAM_CONCENTRATOR;





uint8_t g_Max_Source_Route_Depth_c = MAX_SOURCE_ROUTING_DEPTH;

#if(g_NWK_LINK_STATUS_d == 1 )
uint32_t g_nwk_link_status_period = g_NWK_LINK_STATUS_PERIOD;
#endif



uint16_t g_No_Of_Data_Txns_Bfore_Nvm_Save_c = g_NO_OF_DATA_TXNS_BFORE_NVM_SAVE_c;

/* g_Max_Fragment_Size variable contains the maximum size the application can
send through fragmentation*/

uint16_t g_Max_Fragment_Size_c = FRAGMENT_SIZE;

uint8_t a_Max_Number_Of_End_Points_c = g_MAX_NO_OF_ENDPOINTS_c;

/*******************************************************************************
 * Private Memory declarations
 *******************************************************************************/


/*******************************************************************************
 * Private Function Prototypes
 *******************************************************************************/

/* None */

/*******************************************************************************
 * Public Functions Definitions
 *******************************************************************************/







void InitStackTables( void )
{
#if 0
    InitializeStartNVMLocation(g_NVM_TABLES_START_OFFSET_c , g_NVM_SAS_TABLE_START_OFFSET_c );
    InitStackTablesPointer( ga_Stack_Tables_Data,
                            g_STACK_TABLES_MEMORY_SIZE_c );
    InitgpNeighborTable( g_MAX_NEIGHBOR_TABLE_ENTRIES_c, g_NEIGHBOR_TABLE_OFFSET_c );

    InitgpPersistentNIB( g_PERSISTENT_NIB_OFFSET_c );

#if ( g_ZIGBEE_SECURITY_d == ENABLE )
    InitgpSecurityMaterialDesc(g_MAX_NWK_KEY_c, g_SECURITY_MATERIAL_DESCRIPTOR_OFFSET_c );
#endif      /* ( g_ZIGBEE_SECURITY_d == ENABLE ) */

    InitgpZDOInfoBase( g_ZDO_INFORMATION_BASE_OFFSET_c );

    InitgpAddressMapTable( g_MAX_ADDR_MAP_ENTRIES_c, g_ADDRESS_MAP_TABLE_OFFSET_c );

#if( g_NWK_MESH_ROUTING_d == ENABLE )
    InitgpRouteTable( g_MAX_ROUTE_TABLE_ENTRIES_c, g_ROUTE_TABLE_OFFSET_c );
#endif      /* ( g_NWK_MESH_ROUTING_d == ENABLE ) */


#if ( g_NWK_MANY_TO_ONE_ROUTING_d == ENABLE )
#if (g_NWK_ROUTE_RECORD_TABLE_d == ENABLE )
    InitgpRouteRecordTable( g_MAX_ROUTE_RECORD_TABLE_ENTRIES_c,
                            g_ROUTE_RECORD_TABLE_OFFSET_c  );
#endif //g_NWK_ROUTE_RECORD_TABLE_d
#endif      /* ( g_NWK_MANY_TO_ONE_ROUTING_d == ENABLE ) */


//#if ( g_APS_BINDING_CAPACITY_c == ENABLE )
    if(Binding_Configuration.APS_binding_supported == ENABLE){
        InitgpBindingTable( g_MAX_BINDING_TABLE_ENTRIES_c,
                           g_BINDING_TABLE_OFFSET_c );
    }

    //#endif      /* ( g_APS_BINDING_CAPACITY_c == ENABLE ) */


#if ( g_APS_GROUP_DATA_RX_d == ENABLE )
    InitgpGroupTable( g_MAX_GROUP_TABLE_ENTRIES_c, g_GROUP_TABLE_OFFSET_c );
#endif      /* ( g_APS_GROUP_DATA_RX_d == ENABLE ) */


#if ( g_APS_LAYER_SEC_PROCESSING_d == ENABLE )
    InitgpLinkKeyTable( g_MAX_LINK_KEY_ENTRIES_c, g_LINK_KEY_TABLE_OFFSET_c );

    InitgpLinkKeyFCtable( g_MAX_LINK_KEY_ENTRIES_c, g_LINK_KEY_COUNTER_OFFSET_c );

#if ( ( g_TRUST_CENTRE_d == ENABLE ) && ( g_USE_HASH_KEY == DISABLE ) )
    InitgpAppLinkKeyTable( g_MAX_APP_LINK_KEY_ENTRIES_c,
                           g_APP_LINK_KEY_TABLE_OFFSET_c );
#endif       /* ( ( g_TRUST_CENTRE_d == ENABLE ) && ( g_USE_HASH_KEY == DISABLE ) ) */
#endif      /* ( g_APS_LAYER_SEC_PROCESSING_d == ENABLE ) */

	InitMaxNumberOfEndpoints(a_Max_Number_Of_End_Points_c);


#if ( g_CBKE_INFO_TABLE_NVM_STORAGE_d == ENABLE )
#if (g_KEY_ESTABLISHMENT_CLUSTER_ENABLE_d == 1)
    InitAppCBKETable(g_APP_CBKE_TABLE_OFFSET_c);
#endif
#endif
//	InitNVMOffset(g_NVM_TABLES_START_OFFSET_c);
//	InitAppNumberOfDataTxnBforeNVMSave(g_NO_OF_DATA_TXNS_BFORE_NVM_SAVE_c);
#endif
}


/******************************************************************************/
//#if (g_APP_PROFILE_ID_c== 0xc05e)
#if 0
void ZLL_SaveEssentialParams(uint8_t *pNVM_params , uint8_t size)
{
#if(ZIGBEE_LIB_MEMORY_OPTIMIZATION==1)
  if((pNVM_params != g_NULL_c )&& (size != 0x00)){
    size = (size %2) ? (size + 1 ) : size;
    halCommonWriteToNvm(pNVM_params,
                  g_APP_ZLL_NVM_PARAMS_OFFSET_c + g_NVM_TABLES_START_OFFSET_c,
                        size);
  }
#endif
}

/*******************************************************************************/
void ZLL_RestoreEssentialParams(uint8_t *pNVM_params , uint8_t size)
{
#if(ZIGBEE_LIB_MEMORY_OPTIMIZATION==1)
  if((pNVM_params != g_NULL_c) && (size != 0x00)){
      size = (size %2) ? (size + 1 ) : size;
      halCommonReadFromNvm(pNVM_params,g_APP_ZLL_NVM_PARAMS_OFFSET_c + g_NVM_TABLES_START_OFFSET_c,
                           size);
  }
#endif
}

#endif /* #if (g_APP_PROFILE_ID_c== 0xc05e)*/

/*******************************************************************************
 * Private Functions Definitions
 *******************************************************************************/



/* None */


/*******************************************************************************
 * End Of File
 *******************************************************************************/
