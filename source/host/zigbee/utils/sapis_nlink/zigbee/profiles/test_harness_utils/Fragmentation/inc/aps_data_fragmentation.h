/*******************************************************************************
 * aps_data_fragmentation.h
 ********************************************************************************
 * Program Description:
 *  This file exposes the APS main function which needs to be invoked from the
 *  infinite while loop implemented in the of the main() function. This file
 *  also exposes a function to know the status of the APS Layer which is
 *  required by the Idle task.
 *
 *******************************************************************************/

#ifndef APS_DATA_FRAGMENTATION_H
#define APS_DATA_FRAGMENTATION_H

/*******************************************************************************
 * Includes
 *******************************************************************************/

/*None*/


/*******************************************************************************
 * Global Constants
 *******************************************************************************/

//#if(APS_FRAGMENTATION_CAPABILITY == ENABLE)
#define BIT_SET(var,bitno) ((var) |= 1 << (bitno))
#define BIT_CLR(var,bitno) ((var) &= ~(1 << (bitno)))



/* Lower Nibble of the Tx option as defined as per specification*/
#define	TX_OPTIONS							TxOptions.u8State
#define	TX_OPT_SEC_ENABLE					TxOptions.iBits.BIT00
#define	TX_OPT_USE_NWK_KEY					TxOptions.iBits.BIT01
#define	TX_OPT_REQ_ACK						TxOptions.iBits.BIT02
#define	TX_FRAGMENT_PERMIT					TxOptions.iBits.BIT03





/*Total Number Blocks of One Transmission Window : txWinSize*/
/*Index of the Block which initiate a new Transmission Window :txWinIndex*/
/*Index of the Current Block which is getting Transmitted:txBlockIndex*/
/*Delay between each block :interFrameDelay*/
/*Data payload in the Transmission : dataPayLoad*/
/*This shall be used in case if there are any discarded packet
 * and also server for testing purpose : discardedFrame*/
/*Relay counter shall be keep track of relaying of Block in Transmission Window : relayCounter*/
/* Total number of blocks in the Transmission : totalBlocks*/
/* Total number of retries of the Transmission Window : retries*/
/* This shall be used locate the index of the data to be sent : lastMemLoc*/
/*
 * This shall be used know short address of device,
 * Which has sent the first Fragmented packet : frgmtDevEna
 */
/*
 * This shall be used know short address of device,
 * Which has sent the first Fragmented packet : dataLength
 */
#define DEF_TRANSMISSION_WINDOW_SIZE			0x08
#define MAXIMUM_TRANSIMISSION_WINDOW_SIZE       0x08
#define DEF_TRANSMISSION_WINDOW_INDEX			0x00
#define DEF_TRANSMISSION_BLOCK_INDEX			0x00
#define DEF_INTERFRAME_DELAY					0x01

#define DEF_DISCARDED_BLOCK						0xFF
#define DEF_RELAY_COUNTER						0x00
#define DEF_TOTAL_BLOCKS						0x00
#define DEF_RETRY_COUNTER						0x00
#define DEF_DISCARD_FRAME						0xFF
#define DEF_DISCARD_ONE_FRAME			  	    0xFF
#define DEF_LASTFILLED_MEM_LOC					0x00
#define DEF_DEVICE_ADDRESS						0xFFFF
#define DEF_TOTAL_LENGTH						0x00

#define SET_TRANSMISSION_WINDOW_SIZE			0x01
#define SET_TRANSMISSION_WINDOW_INDEX			0x02
#define SET_TRANSMISSION_BLOCK_INDEX			0x03
#define SET_INTERFRAME_DELAY					0x04
#define SET_DATA_PAYLOAD_SIZE					0x05
#define SET_DISCARDED_BLOCK						0x06
#define SET_RELAY_COUNTER						0x07
#define SET_RETRY_COUNTER						0x08
#define SET_LASTFILLED_MEM_LOC					0x09
#define SET_DEVICE_ADDRESS						0x0A
#define SET_TOTAL_LENGTH						0x0B
#define END_FRAGMENTATION_OPTS                  0x0C

#define MAX_FRAGMENTED_RETRIES					0x04

#define INITIAL_APS_ACK_STATUS                          0xFF

enum fragmentationEvents {
    FRAGMENTATION_TX_START,
    FRAGMENTATION_TX_ABORTED,
    FRAGMENTATION_TX_COMPLETE,
    FRAGMENTATION_RX_START,
    FRAGMENTATION_RX_ABORTED,
    FRAGMENTATION_RX_COMPLETE
};
#define INITIAL_APS_ACK_STATUS                          0xFF

//#endif  						/*(APS_FRAGMENTATION_CAPABILITY == ENABLE)*/


/*******************************************************************************
 * Type Definitions
 *******************************************************************************/

/*******************************************************************************
 * Memory Declarations
 *******************************************************************************/
//#if(APS_FRAGMENTATION_CAPABILITY == ENABLE)


/*******************************************************************************
 frgmtDetails_Tag

 This Structure is the Key for Fragmentation feature. This structres
 hold all the required information to ease the process of fragmentation

 *******************************************************************************/
typedef struct frgmtDetails_Tag {

    /*Total Number Blocks of One Transmission Window*/
    uint8_t txWinSize;
    /*Index of the Block which initiate a new Transmission Window*/
    uint8_t txWinIndex;
    /*Index of the Current Block which is getting Transmitted*/
    uint8_t txBlockIndex;
    /*Delay between each block */
    uint8_t interBlockFrameDelay;
    /*Data payload in the Transmission*/
    uint8_t blockDataLength;
    /*This shall be used in case if there are any discarded packet*/
    uint8_t discardedFrame;
    /*Relay counter shall be keep track of relaying of Block in Transmission Window*/
    uint8_t relayCounter;
    /* Total number of blocks in the Transmission*/
    uint8_t totalBlocks;
    /* Total number of retries of the Transmission Window*/
    uint8_t retries;
//#if ( g_FRAGMENTATION_TEST_d == 1 )
//    /* This shall be used locate the index of the data that should not to be sent*/
//    uint8_t testDiscardFrame;
//    /* This shall be used locate the index of the data that should not to be sent*/
//    uint8_t testDiscardFrameOne;
//#endif /*#if ( g_FRAGMENTATION_TEST_d = 1 )*/

    uint16_t lastMemLoc;
    /*
     * This shall be used know short address of device,
     * Which has sent the first Fragmented packet
     */
    uint16_t frgmtReceivedShortAddress;
    /*
     * This shall be used know short address of device,
     * Which has sent the first Fragmented packet
     */
    uint16_t dataLength;
} __attribute__((__packed__))frgmtDetails_t;





#if ( g_FRAGMENTATION_TEST_d == 1 )
extern FragmentTest_t fragmentTest;
#endif /* ( g_FRAGMENTATION_TEST_d == 1 )*/



/*
 *This Shall have the information of the Window
 *And the Block information for which the  False
 *APS ACK
 */
typedef struct frgmtFalseAck_Tag {
    uint8_t numberOfErrorWin;
    uint8_t errorWindow[6];
    uint8_t ackWindow[6];
} __attribute__((__packed__))frgmtFalseAck_t;

extern sBitLocater_t TxOptions, fragmentOptions;

//extern frgmtFalseAck_t frgmtFalseAck;
extern frgmtDetails_t frgmtDetails;
extern frgmtDetails_t const defFragmentParameters;
extern uint8_t *pTxFragmentBuffer;
extern uint8_t *pRxFragmentBuffer;
extern uint8_t frgmtParamSet;

#if ( g_FRAGMENTATION_TEST_d == 1 )
extern FragmentTest_t const defFragmentTest ;
#endif

 /* This structure provides flexibility of enabling or disabling 
Fragmentation support,  */
typedef struct Fragmentation_Configuration_Tag{
    uint8_t APS_Fragmentation_Supported_d;
}__attribute__((__packed__))Fragmentation_Configuration_t;

 /* This structure provides flexibility of enabling or disabling 
Fragmentation support */
extern Fragmentation_Configuration_t Fragmentation_Configuration;

//#endif  						/*(APS_FRAGMENTATION_CAPABILITY == ENABLE)*/

/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/
//#if(APS_FRAGMENTATION_CAPABILITY == ENABLE)

 






/*******************************************************************************
 * APS_rstFrgmtDetails
 ********************************************************************************
 *   Return Value:
 *   None
 *
 *   Parameters:
 *   Input parameter:
 *   None
 *
 *   Output parameter:
 *
 *   Scope of the function:
 *   This is a global function.
 *
 *   Purpose of the function:
 *   This function shall retain the set fragmentation parameters
 *
 *   Before the function is called:
 *   None
 *
 *   After the function is called:
 *   None
 *
 *******************************************************************************/
void APS_rstFrgmtDetails(void);

/*******************************************************************************
 * fragmentationEvent
 ********************************************************************************
 *   Return Value:
 *   None
 *
 *   Parameters:
 *   Input parameter:
 *       uint8_t   fragmentationEvents shall give the necessary indication to the
 * 				   application layer.
 *       uint8_t bufferId: This parameter holds the information about what
 *       status needs to be indicated in the to the  application.
 *
 *   Output parameter:
 *   		START , COMPLETED & ABORTED
 *
 *   Scope of the function:
 *   This is a global function.
 *
 *   Purpose of the function:
 *   This function shall send the all indication regarding fragmentation to the
 *	 application layer
 *
 *   Before the function is called:
 *   None
 *
 *   After the function is called:
 *   Free Buffer
 *
 *******************************************************************************/
void fragmentationEvent(uint8_t fragmentationEvents, uint8_t bufferId,
        uint8_t* pFrgmtBuffer);

//#endif
/*******************************************************************************
 * End Of File
 *******************************************************************************/

#endif                                      /*aps_data_fragmentation*/
