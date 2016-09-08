#include "stack_functionality.h"
#include "stack_util.h"
#include "zdo_interface.h"
#include "apsde_interface.h"
#include "buffer_management.h"
#include "memory_utility.h"
//#include "aps_data_services.h"
//#include "aps_main.h"
//#include "sw_timer.h"
//#include "timer_id.h"
//#include "stack_timer_callback.h"
//#include "stack_configuration.h"
#include "FragmentationUtility.h"
#include "stack_common.h"
//#include "aps_data_fragmentation.h"


sBitLocater_t  fragmentOptions;
uint8_t frgmtParamSet;
#if ( g_FRAGMENTATION_TEST_d == 1 )
FragmentTest_t fragmentTest;
#endif /* ( g_FRAGMENTATION_TEST_d == 1 )*/

uint8_t fragmentBuffer[FRAGMENT_SIZE];

void ZigBeeFragmentMessageSentHandler(ZigBeeStatus_t status )
{
   
    

}
/******************************************************************************/
void ZigBeeFragmentMessageSent(uint8_t blockTransmitted)
{
    
}


/******************************************************************************/
void ZigBeeIncomingFragmentMessage(uint8_t *pFragmentMsg,uint8_t msgLength,
                                   ZigBeeStatus_t status,
                                   uint8_t blocknoReceived,
                                   uint16_t SourceAddress)
{
    
}

/******************************************************************************/
void ZigBeeFragmentIncomingMessageHandler(uint8_t *pFragmentMsg,uint8_t msgLength,
                                   ZigBeeStatus_t status)
{

    
}