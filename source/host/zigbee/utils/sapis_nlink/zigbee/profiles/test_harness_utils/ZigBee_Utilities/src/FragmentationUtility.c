/*******************************************************************************
 fragmentationutility.c

********************************************************************************
* Program Description:
*
* @brief  This module contains the Fragmentation Utility APIs
* which will be used by the Application Layer
*
********************************************************************************
* Includes
*******************************************************************************/
#include "stack_functionality.h"
#include "stack_common.h"
#include "stack_util.h"
#include "zdo_interface.h"
#include "app_configuration_default.h"
#include "apsde_interface.h"
#include "buffer_management.h"
#include "memory_utility.h"
#include "FragmentationUtility.h"
#include "stack_common.h"
#include "aps_data_fragmentation.h"

extern frgmtDetails_t frgmtDetails;
 uint8_t *pTxFragmentBuffer;
 uint8_t *pRxFragmentBuffer;
/*******************************************************************************
* Global Constants
*******************************************************************************/
/* None */

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/* None */

/*******************************************************************************
* Public Memory declarations
*******************************************************************************/

sBitLocater_t  fragmentOptions;
uint8_t frgmtParamSet;
#if ( g_FRAGMENTATION_TEST_d == 1 )
FragmentTest_t fragmentTest;
#endif /* ( g_FRAGMENTATION_TEST_d == 1 )*/

uint8_t fragmentBuffer[FRAGMENT_SIZE];


/*******************************************************************************
* Public Functions
*******************************************************************************/
void  ZigBeeFragment_SetFragmentOptions( uint8_t *pBufferRx)
{
//    uint8_t index;
    uint16_t indexFillFragment = 0;


    /* Complete fragmentation option is copied*/
    FRA_CMD = *(pBufferRx + 3);
    /* FRA_CLR_DATA = Bit 1 in fragment opt*/
    if (ENABLE == FRA_CLR_DATA) {
        if (*pBufferRx + 2 == 0x02) {
            /* 0x00 is set to fragment buffer*/
            memUtils_memSet(fragmentBuffer, (*pBufferRx + 4),
                    FRAGMENT_SIZE);
        }
        else {
            memUtils_memSet(fragmentBuffer, 0x0FF, FRAGMENT_SIZE);
        }
        *(pBufferRx + 3) = FRA_CLR_DATA;
    }
    else if (ENABLE == FRA_STORE_USER_DATA) {
       /* storing the required fragmentation options*/
        memUtils_memCopy(&fragmentBuffer[ZigBeeGetFragmentLastmemoryLocation()],
                (pBufferRx + 4), (*(pBufferRx + 2) - 0x01));
        ZigBeeSetFragmentLastmemoryLocation((*(pBufferRx + 2) - 0x01));
        *(pBufferRx + 3) = FRA_STORE_USER_DATA;
    }
    else if (ENABLE == FRA_FILL_DEFAULT_DATA) {
        for (indexFillFragment = 0; indexFillFragment <
             FRAGMENT_SIZE; indexFillFragment++) {
            fragmentBuffer[indexFillFragment]
                    = (uint8_t) indexFillFragment + 1;
        }
        *(pBufferRx + 3) = FRA_FILL_DEFAULT_DATA;
    }

    else if (ENABLE == FRA_SET_FRAGMENTATION_PARAMETERS) {
        ZigBeeSetFragmentTxWinSize(*(pBufferRx + 4));
        if ((*(pBufferRx + 5) > MAX_PAY_LOAD_SIZE) && (*(pBufferRx + 5)
                == ZERO)) {
            ZigBeeSetFragmentBlockDataLength(DEF_FRAGMENT_DATA_PAYLOAD_SIZE);
        }
        else {
            ZigBeeSetFragmentBlockDataLength( *(pBufferRx + 5));
        }
        if (*(pBufferRx + 2) == 0x06) {
#if ( g_FRAGMENTATION_TEST_d == 1 )
            fragmentTest.testDiscardFrame = *(pBufferRx + 6);
            fragmentTest.testDiscardFrameOne = *(pBufferRx + 7);
#endif
           // frgmtDetails.interBlockFrameDelay = *(pBufferRx + 8);
            ZigBeeSetFragmentInterframeDelay(*(pBufferRx + 8));
            frgmtParamSet = 2;
        }
        else {

#if ( g_FRAGMENTATION_TEST_d == 1 )
            fragmentTest.testDiscardFrame = *(pBufferRx + 6);
            //frgmtDetails.interBlockFrameDelay = *(pBufferRx + 7);
            ZigBeeSetFragmentInterframeDelay(*(pBufferRx + 7));

            if (fragmentTest.testDiscardFrame == 0xFF) {
                frgmtParamSet = 0xFF;
            }
            else {
                frgmtParamSet = 1;
            }
#endif
        }
    }
//    else if (ENABLE == FRA_SEND_FALSE_ACK) {
//
//        frgmtFalseAck.numberOfErrorWin = *(pBufferRx + 4);
//        if (frgmtFalseAck.numberOfErrorWin > 0) {
//            frgmtParamSet = 10;
//        }
//        for (index = 0; index < frgmtFalseAck.numberOfErrorWin; index++) {
//            frgmtFalseAck.errorWindow[index] = *(pBufferRx + 5 + index);
//            frgmtFalseAck.ackWindow[index] = *(pBufferRx + 6 + index);
//        }
//
//    }
    *(pBufferRx + 3) = FRA_SET_FRAGMENTATION_PARAMETERS;
}
/******************************************************************************/

void AppFragmentAllocateSpace( void )
{
  APS_AllocateFragmentationSpace(fragmentBuffer,fragmentBuffer);
}

bool ZigBeeSetFragmentTxWinSize(uint8_t txWinSize)
{
    /* apsMaxWindowSize range 1- 8 */
    if( txWinSize >= 0x01 && txWinSize <= MAXIMUM_TRANSIMISSION_WINDOW_SIZE)  {
        frgmtDetails.txWinSize = txWinSize;
        return g_TRUE_c;
    }
    else{
        return g_FALSE_c;
    }
}

void APS_AllocateFragmentationSpace(uint8_t* fragTxBuffer, uint8_t *fragRxBuffer)
{
    pTxFragmentBuffer = fragTxBuffer;
    pRxFragmentBuffer = fragRxBuffer;
}

void ZigBeeSetFragmentInterframeDelay(uint8_t interframeDelay)
{
    /* default range 0x00 to 0xff.
    the standard delay, in milliseconds, between sending two blocks of a
    fragmented transmission*/
    frgmtDetails.interBlockFrameDelay = interframeDelay;
}
/*******************************************************************************
* Private Functions
*******************************************************************************/

/* None */

/*******************************************************************************
 * End Of File
 ******************************************************************************/

