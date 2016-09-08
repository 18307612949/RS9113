#include "bt_common.h"
#include "onebox_bt_core.h"
#include "onebox_bt_pktpro.h"

static struct onebox_osi_bt_ops osi_bt_ops = {
	.onebox_core_init            = core_bt_init,
	.onebox_core_deinit          = core_bt_deinit,
	.onebox_indicate_pkt_to_core = bt_core_pkt_recv,
	.onebox_dump                 = onebox_print_dump,
	.onebox_bt_xmit              = bt_xmit,
	.onebox_snd_cmd_pkt          = onebox_bt_mgmt_pkt_recv,
	.onebox_send_pkt             = send_bt_pkt,
	.onebox_bt_cw                = bt_cw,
 	.onebox_bt_ber               = bt_ber,
	.onebox_bt_read_reg_params   = bt_read_reg_params,
};

struct onebox_osi_bt_ops *onebox_get_osi_bt_ops(void)
{
	return &osi_bt_ops;
}
EXPORT_SYMBOL(onebox_get_osi_bt_ops);
