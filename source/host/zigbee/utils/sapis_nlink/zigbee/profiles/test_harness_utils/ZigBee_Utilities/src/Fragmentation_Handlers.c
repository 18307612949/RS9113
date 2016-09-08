/*******************************************************************************
 fragmentation_handlers
********************************************************************************
* Program Description:
*
* @brief  This module contains the Fragmentation handler APIs
* which will be used by the Application Layer
*
********************************************************************************
* Includes
*******************************************************************************/

#include <stdio.h>
#include "stack_common.h"
#include "zdo_interface.h"
#include "apsde_interface.h"
#include "buffer_management.h"
#include "aps_data_fragmentation.h"
/*******************************************************************************
* Global Constants
*******************************************************************************/

frgmtDetails_t frgmtDetails;
/* None */

/*******************************************************************************
* Public Memory declarations
*******************************************************************************/

/* None */

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/* None */


/*******************************************************************************
* Public Functions
*******************************************************************************/

void ZigBeeFragmentMessageSentHandler(ZigBeeStatus_t status )
{
#if (g_CLI_SUPPORT_d == 1 )
    if(status == ZigBee_Fragment_Tx_Aborted){

        printf(" Fragmentation Aborted \n\r");
    }
    else if(status  == ZigBee_Fragment_Tx_Complete){

        printf(" Fragmented packet transmission completed");
    }
#else
     /* none */
#endif  /* g_CLI_SUPPORT_d */

}
/******************************************************************************/
void ZigBeeFragmentMessageSent(uint8_t blockTransmitted)
{
#if (g_CLI_SUPPORT_d == 1 )
    printf(" Last transmitted packet block no %d from APS Layer \n\r",
           blockTransmitted );
#else
     /* none */
#endif  /* g_CLI_SUPPORT_d */
}


/******************************************************************************/
void ZigBeeIncomingFragmentMessage(uint8_t *pFragmentMsg,uint8_t msgLength,
                                   ZigBeeStatus_t status,
                                   uint8_t blocknoReceived,
                                   uint16_t SourceAddress)
{
    /* pFragmentMsg pointer should be used only, no manipulation shd be done on
    this pointer*/
#if (g_CLI_SUPPORT_d == 1 )
    uint8_t *pLocalPointer = pFragmentMsg;

    if((pLocalPointer != g_NULL_c) && (msgLength != 0x00)){
        printf("\n\rFragment Message Details :\n\r");
        printf("============================================================\n\r");
        printf(" Fragment message received from %04x\n\r",SourceAddress);
        /* received block no */
        printf(" Blockno : %d \n\r",blocknoReceived);
        /* received data */
        printf(" Data :");
        for(uint8_t i= 0; (i < msgLength) && (pLocalPointer != g_NULL_c) ; i++){
            printf( " %x ", (*pLocalPointer) );
            if(i % 10 == 0x00){
                printf("\n\r");
            }/* end of if*/
            pLocalPointer++;
        }/* end of for*/
    }/*end of if*/
#else
     /* none */
#endif  /* g_CLI_SUPPORT_d */
}

/******************************************************************************/
void ZigBeeFragmentIncomingMessageHandler(uint8_t *pFragmentMsg,uint8_t msgLength,
                                   ZigBeeStatus_t status)
{
#if (g_CLI_SUPPORT_d == 1 )
  /*  Used Only for Sample Applications */
    printf("\n\rFragmentation Reception Process Status\n\r");
    printf("*******************************************************\n\r");
    if(status == ZigBee_Fragment_Rx_Aborted){
		printf(" Reception Aborted \n\r");
	}
	else if(status == ZigBee_Fragment_Reception_Completed){
		printf(" Data After Complete Reception and reassembling \n\r");
		for(uint8_t index = 0x00;((index < msgLength) && (pFragmentMsg != g_NULL_c));
			index++){
			printf(" %x ", *pFragmentMsg++);
			/* To print data in next line of 10 characters */
			if(index % 10 == 0x00){
				printf(" \n\r");
			}
		}
	}
#else
     /* none */
#endif  /* g_CLI_SUPPORT_d */
}

/******************************************************************************/
void ZigBeeSendPollConfirmationCB( uint8_t bufferIndex)
{

#if(g_FRAGMENTATION_TEST_d == ENABLE)

    if (ZigBeeIsFragmentReceptionInProgress() != ENABLE ) {
        /* Sending the poll confirmation to the application */
        ZDP_APPresponse(bufferIndex);
    }
    else {
        buffMgmt_freeBuffer(bufferIndex, 225);
    }

#else
    /* Sending the poll confirmation to the application */
    ZDP_APPresponse(bufferIndex);
#endif

}

sBitLocater_t fragmentInProgress;
#define FRA_START_TX						fragmentInProgress.iBits.BIT05
#define FRA_START_RX						fragmentInProgress.iBits.BIT06
#define FRAGMENT_RETRY                      fragmentInProgress.iBits.BIT07
bool ZigBeeIsFragmentReceptionInProgress( void )
{
  return ((FRA_START_RX != ENABLE) ? DISABLE : ENABLE );
}
/******************************************************************************/
uint8_t ZigBeeGetFragmentTxWinSize( void )
{
    /* Since,Fragmentation not supported */
    return frgmtDetails.txWinSize;
}

uint8_t ZigBeeGetFragmentInterBlockFrameDelay( void )
{
    /* Since,Fragmentation not supported */
    return frgmtDetails.interBlockFrameDelay;
}

void ZigBeeSetFragmentBlockDataLength(uint8_t blockDataLength)
{
   frgmtDetails.blockDataLength = blockDataLength;
}

uint16_t ZigBeeGetFragmentLastmemoryLocation( void )
{
  return frgmtDetails.lastMemLoc;
}

void ZigBeeSetFragmentLastmemoryLocation( uint16_t lastMemoryLoc )
{
  frgmtDetails.lastMemLoc += lastMemoryLoc;
}

/*******************************************************************************
* Private Functions
*******************************************************************************/
/* None */

/*******************************************************************************
 * End Of File
 ******************************************************************************/
