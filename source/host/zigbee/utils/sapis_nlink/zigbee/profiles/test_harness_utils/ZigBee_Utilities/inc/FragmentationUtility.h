/*******************************************************************************
 * FragmentationUtility.h
 ********************************************************************************
 * Program Description:
 *	This file contains all the support functions and structures of the stack
 *	to be used by application.
 *
 *******************************************************************************/

#ifndef FRAGMENTATION_UTILITY_H
#define FRAGMENTATION_UTILITY_H

/*******************************************************************************
 * Includes
 *****************************************************************************/
/* None */

/*******************************************************************************
 * Global Constants
 *******************************************************************************/

/* Options used for automating Fragmentation purpose are as Defined below*/
#define FRA_CMD							  	fragmentOptions.u8State
#define	FRA_CLR_DATA					  	fragmentOptions.iBits.BIT00
#define	FRA_STORE_USER_DATA			  	  	fragmentOptions.iBits.BIT01
#define	FRA_FILL_DEFAULT_DATA			  	fragmentOptions.iBits.BIT02
#define	FRA_SET_FRAGMENTATION_PARAMETERS  	fragmentOptions.iBits.BIT03
#define	FRA_SEND_FALSE_ACK					fragmentOptions.iBits.BIT04



/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/

/******************************************************************************
 * AppFragmentAllocateSpace
 ******************************************************************************
 *
 * Return Value :  none
 *	
 *
 *
 * Parameters   :
 *
 * Input Parameters:
 *  	None
 * Output Parameters:
 *      none
 *
 * Scope of the function:
 *	This is called by the Application
 * Purpose of the function:
 *	
 *
 * Before the function is called:
 *   None
 *
 * After the function is called:
 *   None
 ******************************************************************************/
void AppFragmentAllocateSpace( void );


/*******************************************************************************
 * End Of File
 *******************************************************************************/
#endif                                           /*FRAGMENTATION_UTILITY_H*/