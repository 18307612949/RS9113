/**
 * @file onebox_gpio.c
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


#ifdef GPIO_HANDSHAKE
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include "onebox_common.h"
#include "onebox_sdio_intf.h"
#include "onebox_imx51_gpio.h"
#include "onebox_zone.h"

/**
 * gpio_deinit(): to de initialize gpio
 * @return:
 * @params:
 */
void gpio_deinit(void)
{
	gpio_free(RSI_GPIO_READ);
	gpio_free(RSI_GPIO_WRITE);
}

/**
 * gpio_init(): to initialize gpio
 * @return:
 * @params:
 */

void gpio_init(void)
{
	int rc = 0;
	struct pad_desc pad_gpio_write,pad_gpio_read;
#ifdef RSI_IMX51
	char *read_gpio="GPIO1_3";
	char *write_gpio="GPIO1_9";
#endif

	rc = gpio_request(RSI_GPIO_WRITE, write_gpio);
	if (rc) {
		ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s: failed(%d) setup %s \n", __func__, rc,write_gpio));
		return ;
	}

	rc = gpio_request(RSI_GPIO_READ, read_gpio);
	if (rc) {
		ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s: failed(%d) setup %s \n", __func__, rc,read_gpio));
		return ;
	}

	rc = gpio_direction_input(RSI_GPIO_READ);
	if (rc) {
		ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s: failed(%d) setup direction %s \n", __func__, rc,read_gpio));
		return ;
	}

	rc = gpio_direction_output(RSI_GPIO_WRITE, 0);
	if (rc) {
		ONEBOX_DEBUG( ONEBOX_ZONE_ERROR, ("%s: failed(%d) setup direction %s \n", __func__, rc,write_gpio));
		return ;
	}

	pad_gpio_write = (struct pad_desc)MX51_PAD_GPIO_1_3__GPIO_1_3_V;
	mxc_iomux_v3_setup_pad(&pad_gpio_write);
	pad_gpio_read = (struct pad_desc)MX51_PAD_GPIO_1_9__GPIO_1_9_V;
	mxc_iomux_v3_setup_pad(&pad_gpio_read);

}


/**
 * set_host_status() - This function is used to write to Imx51 GPIO1_3.
 *
 * @value: The value of the host gpio either TRUE or FALSE.
 *
 * Return: None.
 */
void set_host_status (int value)
{
	__gpio_set_value(RSI_GPIO_WRITE, value);
}

/**
 * get_device_status() - This function is used to read the GPIO1_9, Calls Device Specific Functions.
 *
 * Return: TRUE if High,else FALSE if LOW.
 */
int get_device_status(void)
{
	return __gpio_get_value(RSI_GPIO_READ) ;
}

#endif


