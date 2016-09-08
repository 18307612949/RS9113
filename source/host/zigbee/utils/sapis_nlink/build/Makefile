#  Make File

rm=/bin/rm -f
CC= cc
AR = ar
DEFS=  
PROGNAME= rsi_wc_app
INCLUDES=  -I../include 
LINKS= -lpthread -lrt


API_PATH = ../
ZB_CORE_SRC = $(API_PATH)/driver/
ZB_REF_APPS_SRC=$(API_PATH)/zigbee/

DEFINES= $(INCLUDES) $(DEFS) 
CFLAGS= -g -Wall $(DEFINES) -Os
CFLAGS+= -D RSI_UART_INTERFACE
#debug prints are valid in uart case only
CFLAGS+= -D RSI_ENABLE_DEBUG_PRINT
#CFLAGS+= -D RSI_SPI_INTERFACE
CFLAGS+= -D LINUX_PLATFORM
CFLAGS+= -D RSI_SAMPLE_HAL

ifeq ($(MAKECMDGOALS),ble)
CFLAGS+= -D RSI_BT_ENABLE
CFLAGS+= -D RSI_BLE_ENABLE
endif

ifeq ($(MAKECMDGOALS),bt_classic)
CFLAGS+= -D RSI_BT_ENABLE
CFLAGS+= -D RSI_BLE_ENABLE
endif

ifeq ($(MAKECMDGOALS),wlan_bt_classic)
CFLAGS+= -D RSI_BT_ENABLE
CFLAGS+= -D RSI_BLE_ENABLE
endif

ifeq ($(MAKECMDGOALS),wlan_ble)
CFLAGS+= -D RSI_BT_ENABLE
CFLAGS+= -D RSI_BLE_ENABLE
endif

ifeq ($(MAKECMDGOALS),wlan_dual_mode)
CFLAGS+= -D RSI_BT_ENABLE
CFLAGS+= -D RSI_BLE_ENABLE
endif

ifeq ($(MAKECMDGOALS),zb)
CFLAGS+= -D RSI_ZB_ENABLE
CFLAGS+= -D ZB_DEBUG
CFLAGS+= -D ZB_PROFILE
CFLAGS += -D ENABLE_ZB_NL
CFLAGS+=  -I ../zigbee/profiles/include
CFLAGS+=  -I ../driver/intf/nlink/include/
CFLAGS+=  -I ../zigbee/profiles/utilities/buffer_management/inc
endif


WLAN_OBJS = ../driver/rsi_setregion_countryinfo.o \
							../nwk/socket/rsi_socket.o \
							../driver/rsi_wlan.o \
							../wlan/rsi_wlan_apis.o \
							../nwk/applications/rsi_dns.o \
							../nwk/applications/rsi_ftp.o \
							../nwk/applications/rsi_http_client.o \
							../nwk/applications/rsi_web_socket.o \
							../nwk/applications/rsi_smtp_client.o \
         			../nwk/applications/rsi_multicast.o \
         			../nwk/applications/rsi_mdnsd.o \
							../nwk/applications/rsi_fwup.o \
              ../nwk/applications/http_server/rsi_http_server.o \
							../nwk/applications/http_server/rsi_json_handlers.o \
							../driver/rsi_nwk.o 

COMMON_OBJS = ../common/rsi_common_apis.o \
							../common/rsi_device_init_apis.o \
							../driver/rsi_driver.o \
							../driver/rsi_driver_event_handlers.o \
							../driver/rsi_events.o \
							../driver/rsi_os_none.o \
							../driver/rsi_pkt_mgmt.o \
							../driver/rsi_queue.o \
							../driver/rsi_device_init.o \
							../driver/rsi_scheduler.o \
							../driver/rsi_utils.o \
							../hal/rsi_hal_mcu_interrupt.o \
							../hal/rsi_hal_mcu_ioports.o \
							../hal/rsi_hal_mcu_spi.o  \
							../hal/rsi_hal_mcu_uart.o  \
							../hal/rsi_hal_mcu_timer.o \
							../hal/rsi_hal_mcu_random.o \
							../driver/rsi_common.o  \
							../driver/intf/nlink/src/rsi_nl_app.o \
							../driver/intf/nlink/src/rsi_zigb_linux_apis.o\
                                                        ../driver/intf/nlink/src/rsi_nlink_main.o

ZB_OBJS = $(ZB_CORE_SRC)/rsi_zb.o \
					$(ZB_CORE_SRC)/rsi_zb_utility.o \
					../hal/rsi_timer.o \
				  $(ZB_REF_APPS_SRC)/rsi_zb_apis.o  \

BT_COMMON_OBJS = ../driver/rsi_bt_ble.o \
					 ../bt_ble/rsi_bt_common_apis.o 

BLE_OBJS = ../bt_ble/rsi_ble_gap_apis.o \
					 ../bt_ble/rsi_ble_gatt_apis.o

BT_CLASSIC_OBJS = ../bt_ble/rsi_bt_gap_apis.o \
				  ../bt_ble/rsi_bt_spp_apis.o 
.c.o:
	$(RM) $@
	$(CC) $(CFLAGS) $(LINKS)  -o $*.o  -c $*.c

all: $(PROGNAME)

$(PROGNAME) : $(COMMON_OBJS) $(WLAN_OBJS)
	$(CC) $(CFLAGS) $(LINKS) -o $(PROGNAME) $(COMMON_OBJS) $(LIBS)


wlan: clean $(WLAN_OBJS) $(COMMON_OBJS)

	ar -rvs librps.a $(COMMON_OBJS) $(WLAN_OBJS) 

zb: clean $(ZB_OBJS) $(COMMON_OBJS) $(WLAN_OBJS)

	ar -rvs librps.a $(COMMON_OBJS) $(ZB_OBJS) $(WLAN_OBJS)

ble: clean $(BT_COMMON_OBJS) $(BLE_OBJS) $(COMMON_OBJS)

	ar -rvs librps.a $(BT_COMMON_OBJS) $(COMMON_OBJS) $(BLE_OBJS) 

bt_classic: clean $(BT_COMMON_OBJS) $(BT_CLASSIC_OBJS) $(COMMON_OBJS)

	ar -rvs librps.a $(BT_COMMON_OBJS) $(COMMON_OBJS) $(BT_CLASSIC_OBJS)

wlan_ble: clean $(BT_COMMON_OBJS) $(BLE_OBJS) $(COMMON_OBJS) $(WLAN_OBJS)

	ar -rvs librps.a $(BT_COMMON_OBJS) $(COMMON_OBJS) $(BLE_OBJS) $(WLAN_OBJS)

wlan_dual_mode: clean $(BT_COMMON_OBJS) $(BT_CLASSIC_OBJS) $(BLE_OBJS) $(COMMON_OBJS) $(WLAN_OBJS)

	ar -rvs librps.a $(BT_COMMON_OBJS) $(COMMON_OBJS) $(BT_CLASSIC_OBJS) $(BLE_OBJS) $(WLAN_OBJS)

wlan_bt_classic: clean $(BT_COMMON_OBJS) $(BT_CLASSIC_OBJS) $(COMMON_OBJS) $(WLAN_OBJS)

	ar -rvs librps.a $(BT_COMMON_OBJS) $(COMMON_OBJS) $(BT_CLASSIC_OBJS) $(WLAN_OBJS)

clean:
	$(rm) librps.a $(COMMON_OBJS) $(ZB_OBJS) $(BLE_OBJS) $(BT_COMMON_OBJS) $(BT_CLASSIC_OBJS) $(WLAN_OBJS) $(PROGNAME) core *.o *~
