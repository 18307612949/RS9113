#include <linux/module.h>
#include <linux/kernel.h>
#include "onebox_datatypes.h"
#include "wlan_common.h"

ONEBOX_STATIC int32 onebox_wlan_nongpl_module_init(VOID)
{
	ONEBOX_DEBUG(ONEBOX_ZONE_INIT,(TEXT("onebox_wlan_nongpl_module_init called and registering the wlan nongpl driver\n")));
	return 0;
}

ONEBOX_STATIC VOID onebox_wlan_nongpl_module_exit(VOID)
{
	ONEBOX_DEBUG(ONEBOX_ZONE_INIT,(TEXT("onebox_wlan_nongpl_module_exit called and unregistering the wlan nongpl driver\n")));
	return;
}

module_init(onebox_wlan_nongpl_module_init);
module_exit(onebox_wlan_nongpl_module_exit);
