/******************************************************************************
* DoorLockDevice.h
******************************************************************************
* Program Description:
* This file contains the DoorLock Device
******************************************************************************/

#ifndef _DOOR_LOCK_H_
#define _DOOR_LOCK_H_

#include "ZCL_Foundation.h"

/*-----------------------------------------------------------------------------
* Memory Declarations
*----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* Memory Declarations
*----------------------------------------------------------------------------*/
#if ( DEBUG_TEST == 1 )
extern uint8_t allocatedbuffer[24];
extern uint8_t freebuffer[24][2];
#endif

/*-----------------------------------------------------------------------------
* Function Prototypes
*----------------------------------------------------------------------------*/

void Handle_ZCL_Message( uint8_t buffer_index, APSDE_Data_Indication_t *p_buffer );


#endif 													/* _DOOR_LOCK_H_*/

/*-----------------------------------------------------------------------------
* End Of File
*----------------------------------------------------------------------------*/