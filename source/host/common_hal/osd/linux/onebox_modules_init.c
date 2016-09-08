/**
 * @file onebox_modules_init.c
 * @author 
 * @version 1.0
 *
 * @section LICENSE
 *
 * This software embodies materials and concepts that are confidential to Redpine
 * Signals and is made available solely pursuant to the terms of a written license
 * agreement with Redpine Signals
 *
 * @section DESCRIPTION
 *
 * This file contains all the Linux network device specific code.
 */


#include "onebox_common.h"
#include "onebox_linux.h"
#include "onebox_sdio_intf.h"
#include "onebox_host_intf_ops.h"
#include "onebox_zone.h"

/**
 * This function is invoked when the module is loaded into the
 * kernel. It registers the client driver.
 *
 * @param  VOID.  
 * @return On success 0 is returned else a negative value. 
 */
ONEBOX_STATIC int32 __init onebox_module_init(VOID)
{

	ONEBOX_STATUS status = 0;
	struct onebox_osd_host_intf_operations *osd_host_intf_ops ;

#ifdef USE_USB_INTF
	osd_host_intf_ops = onebox_get_usb_osd_host_intf_operations();
	/* registering the client driver */ 
	if (osd_host_intf_ops->onebox_register_drv() == 0) {
		ONEBOX_DEBUG(ONEBOX_ZONE_INIT,
				(TEXT("%s: Successfully registered USB gpl driver\n"),
				 __func__));
			status = ONEBOX_STATUS_SUCCESS;
	} else {
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
				(TEXT("%s: Unable to register USB driver\n"), __func__));
		status = ONEBOX_STATUS_FAILURE;
		/*returning a negative value to imply error condition*/
	} /* End if <condition> */    
#endif

#ifdef USE_SDIO_INTF
	osd_host_intf_ops = onebox_get_sdio_osd_host_intf_operations();

	if (osd_host_intf_ops->onebox_register_drv() == 0) {
		ONEBOX_DEBUG(ONEBOX_ZONE_INIT,
				(TEXT("%s: Successfully registered SDIO gpl driver\n"),
				 __func__));
			status = ONEBOX_STATUS_SUCCESS;
	} else {
		ONEBOX_DEBUG(ONEBOX_ZONE_ERROR,
				(TEXT("%s: Unable to register SDIO gpl driver\n"), __func__));
		/*returning a negative value to imply error condition*/
		status = ONEBOX_STATUS_FAILURE;
	} /* End if <condition> */    
#endif
	return status;
}/* End of <onebox_module_init> */

/**
 * At the time of removing/unloading the module, this function is 
 * called. It unregisters the client driver.
 *
 * @param  VOID.  
 * @return VOID. 
 */
ONEBOX_STATIC VOID __exit onebox_module_exit(VOID)
{
	struct onebox_osd_host_intf_operations *osd_host_intf_ops;
	
#ifdef USE_USB_INTF
	osd_host_intf_ops = onebox_get_usb_osd_host_intf_operations();
	osd_host_intf_ops->onebox_unregister_drv();
	ONEBOX_DEBUG(ONEBOX_ZONE_INFO,
	             (TEXT("%s: Unregistered the USB driver\n"), __func__));
#endif

#ifdef USE_SDIO_INTF
	osd_host_intf_ops = onebox_get_sdio_osd_host_intf_operations();

	osd_host_intf_ops->onebox_unregister_drv();
	ONEBOX_DEBUG(ONEBOX_ZONE_INFO,
			(TEXT("%s: Unregistered the SDIO driver\n"), __func__));
#endif
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Redpine Signals Inc");
MODULE_DESCRIPTION("Coexistance Solution From Redpine Signals");
MODULE_SUPPORTED_DEVICE("Godavari RS911x WLAN Modules");

module_init(onebox_module_init);
module_exit(onebox_module_exit);
