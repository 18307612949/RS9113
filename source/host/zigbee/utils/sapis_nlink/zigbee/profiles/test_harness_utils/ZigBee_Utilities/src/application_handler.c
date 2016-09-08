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
#include "stack_common.h"
#include "zdo_interface.h"
#include "nlme_interface.h"

/*******************************************************************************
* Global Constants
*******************************************************************************/

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
#if (g_APP_PROFILE_ID_c != 0x0104)
void app_MACTrxUnicastFilureHandler(void)__attribute__((weak));
void app_MACTrxUnicastFilureHandler(void)
{
	return;
}
/******************************************************************************/
void app_No_OF_ResetsHandler(void)__attribute__((weak));
void app_No_OF_ResetsHandler(void)
{
  return;

}
/******************************************************************************/
void app_PersistentMemoryWritesHandler(void)__attribute__((weak));
void app_PersistentMemoryWritesHandler(void)
{
  return;

}
/******************************************************************************/
void app_MACRecBroadcastHandler(void)__attribute__((weak));
void app_MACRecBroadcastHandler(void)
{
  return;

}
/******************************************************************************/
void app_MACRecUnicastHandler(void)__attribute__((weak));
void app_MACRecUnicastHandler(void)
{
  return;

}
/******************************************************************************/
void app_MACTrxUnicastHandler(void)__attribute__((weak));
void app_MACTrxUnicastHandler(void)
{
  return;

}
/******************************************************************************/
void app_MACTrxUnicastRetryHandler(void)__attribute__((weak));
void app_MACTrxUnicastRetryHandler(void)
{
  return;

}

/******************************************************************************/
void app_APSTrxBroadcastHandler(void)__attribute__((weak));
void app_APSTrxBroadcastHandler(void)
{
  return;

}
/******************************************************************************/
void app_APSRecUnicastHandler(void)__attribute__((weak));
void app_APSRecUnicastHandler(void)
{
  return;

}
/******************************************************************************/
void app_APSTrxUnicastRetryHandler(void)__attribute__((weak));
void app_APSTrxUnicastRetryHandler(void)
{
  return;

}
/******************************************************************************/
void app_APSTrxUnicastFailureHandler(void)__attribute__((weak));
void app_APSTrxUnicastFailureHandler(void)
{
  return;

}
/******************************************************************************/
void app_RouteDiscInitiatedHandler(void)__attribute__((weak));
void app_RouteDiscInitiatedHandler(void)
{
  return;

}
/******************************************************************************/
void app_NeighborAddedHandler(void)__attribute__((weak));
void app_NeighborAddedHandler(void)
{
  return;

}
/******************************************************************************/
void app_NeighborRemovedHandler(void)__attribute__((weak));
void app_NeighborRemovedHandler(void)
{
  return;

}
/******************************************************************************/
void app_NeighborStaleHandler(void)__attribute__((weak));
void app_NeighborStaleHandler(void)
{
  return;

}
/******************************************************************************/
void app_JoinIndicationHandler(void)__attribute__((weak));
void app_JoinIndicationHandler(void)
{
  return;

}
/******************************************************************************/
void app_ChildMovedHandler(void)__attribute__((weak));
void app_ChildMovedHandler(void)
{
  return;

}
/******************************************************************************/
void app_NWKFCFailureHandler(void)__attribute__((weak));
void app_NWKFCFailureHandler(void)
{
  return;

}

/******************************************************************************/
void app_APSFC_FailureHandler(void)__attribute__((weak));
void app_APSFC_FailureHandler(void)
{
  return;
}

/******************************************************************************/
void app_APSUnauthorizedKeyHandler(void)__attribute__((weak));
void app_APSUnauthorizedKeyHandler(void)
{
  return;

}
/******************************************************************************/
 void app_NWKDecryptFailuresHandler(void)__attribute__((weak));
 void app_NWKDecryptFailuresHandler(void)
{
  return;

}
/******************************************************************************/

void app_APSDecryptFailuresHandler(void)__attribute__((weak));
void app_APSDecryptFailuresHandler(void)
{
  return;
}
/******************************************************************************/

void app_PacketBufferAllocateFailuresHandler(void)
{
  return;

}
/******************************************************************************/

void app_RelayedUcastHandler(void)__attribute__((weak));
void app_RelayedUcastHandler(void)
{
  return;
}
/******************************************************************************/

void app_PhyToMACQueueLimitReachedHandler(void)__attribute__((weak));
void app_PhyToMACQueueLimitReachedHandler(void)
{
  return;
}
/******************************************************************************/

void app_PacketValidateDropHandler(void)__attribute__((weak));
void app_PacketValidateDropHandler(void)
{
  return;
}
/******************************************************************************/

void app_APSTrxUnicastSuccessHandler  (void)__attribute__((weak));
void app_APSTrxUnicastSuccessHandler  (void) {
  return;
}
/******************************************************************************/

void app_APSRecBroadcastHandler (void)__attribute__((weak));
void app_APSRecBroadcastHandler (void) {
  return;
}
/******************************************************************************/
void app_MACTrxBroadcastHandler(void)__attribute__((weak));
void app_MACTrxBroadcastHandler(void) {
  return;
}
#endif  /* g_APP_PROFILE_ID_c*/
/******************************************************************************/
#if ((g_APP_PROFILE_ID_c != 0x0109 ) || (g_APP_PROFILE_ID_c != 0x0104))
void xmodem_pollCallback(void)__attribute__((weak));
void xmodem_pollCallback(void)
{
  return;
}
#endif   /* (g_APP_PROFILE_ID_c != 0x0109 ) || (g_APP_PROFILE_ID_c != 0x0104)*/
//#if (g_APP_PROFILE_ID_c != 0x0109 ) ||(g_APP_PROFILE_ID_c != 0xc05e)
///******************************************************************************/
//__weak void App_InterPanDataindication (void)
//{
//  return;
//}
///******************************************************************************/
//__weak void App_InterPanDataConfirmation (void)
//{
//  return;
//}
//#endif   /* g_APP_PROFILE_ID_c*/
#ifndef SLEEPY_SENSOR_APP
void ZigBeePollCompleteHandler(ZigBeeStatus status)
{

}
#endif
/*******************************************************************************
* Private Functions
*******************************************************************************/
/* None */

/*******************************************************************************
 * End Of File
 ******************************************************************************/
