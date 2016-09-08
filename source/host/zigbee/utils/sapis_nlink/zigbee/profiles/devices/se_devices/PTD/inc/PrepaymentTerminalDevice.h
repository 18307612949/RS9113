#ifndef _CONFIGURATIONTOOLDEVICE_H_
#define _CONFIGURATIONTOOLDEVICE_H_

#include "ZCL_Foundation.h"

/*-----------------------------------------------------------------------------
* Global Constants
*----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
* Type Definitions
*----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
* Memory Declarations
*----------------------------------------------------------------------------*/
extern uint8_t allocatedbuffer[24];
extern uint8_t freebuffer[24][2];

#if(g_PREPAYMENT_CLUSTER_ENABLE_d == 1)
extern uint8_t ReadMeterSerialNoValue[16];
extern uint8_t ReadSiteIdValue[32];
#endif

/*-----------------------------------------------------------------------------
* Function Prototypes
*----------------------------------------------------------------------------*/

#endif /* _ZCL_TEST_APP_H_*/

/*-----------------------------------------------------------------------------
* End Of File
*----------------------------------------------------------------------------*/
