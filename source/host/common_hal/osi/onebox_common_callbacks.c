#include "onebox_common.h"
#include "onebox_core.h"
#include "onebox_linux.h"
#include "onebox_pktpro.h"

static struct onebox_coex_osi_operations coex_osi_ops =
{
	/* In case if u want to change the order of these member assignments, you should change
	 * the order in the declarations also */
	.onebox_handle_interrupt          = sdio_interrupt_handler,
	.onebox_coex_pkt_processor        = coex_pkt_processor,
	.onebox_dump                      = onebox_print_dump,
	.onebox_device_init               = device_init,
	.onebox_device_deinit             = device_deinit,
	.onebox_coex_sched_pkt_xmit       = coex_sched_pkt_xmit,
	.onebox_coex_transmit_thread      = coex_transmit_thread,
	.onebox_coex_mgmt_frame           = onebox_coex_mgmt_frame,
	.onebox_auto_flash_write          = auto_flash_write,
	.onebox_manual_flash_write        = manual_flash_write,
	.onebox_flash_read                = flash_read,
	.onebox_auto_fw_upgrade           = auto_fw_upgrade,
 	.onebox_bl_write_cmd              = bl_write_cmd,
	.onebox_read_reg_params           = read_reg_parameters,
	.onebox_common_hal_init           = common_hal_init,
	.onebox_common_hal_deinit         = common_hal_deinit,
};

struct onebox_coex_osi_operations *onebox_get_coex_osi_operations(void)
{
	return (&coex_osi_ops);
}
EXPORT_SYMBOL(onebox_get_coex_osi_operations);

