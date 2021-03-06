/******************************************************************************
* AppCBKECertificate.h
******************************************************************************
* Program Description:
* This file contains the Key Establishment cluster App type definations
******************************************************************************/
#ifndef _APP_CBKE_CERTIFICTE_H_
#define _APP_CBKE_CERTIFICTE_H_


/*******************************************************************************
* Global Constants
********************************************************************************/



/*******************************************************************************
* Memory Declarations
********************************************************************************/


/*******************************************************************************
* Type Definitions
********************************************************************************/


/*******************************************************************************
   Function Prototypes
*******************************************************************************/
#if( g_KEY_ESTABLISHMENT_CLUSTER_ENABLE_d == 1 )
uint8_t* App_GetCertificate( void );
uint8_t* App_GetStaticPrivateKey( void );
uint8_t* App_GetCAPublicKey( void );
void App_SetCertificate( uint8_t *pCertificate );
void App_SetStaticPrivateKey( uint8_t *pPrivateKey );
void App_SetCAPublicKey( uint8_t *pPublicKey );

#endif
/*******************************************************************************
* End Of File
*******************************************************************************/
#endif          /* _APP_CBKE_CERTIFICTE_H_ */