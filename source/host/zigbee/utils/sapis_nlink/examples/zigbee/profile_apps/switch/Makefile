# Makefile

rm=/bin/rm -f
CC= cc
zb= zb
DEFS=  
PROGNAME= rsi_wc_app
INCLUDES=  -I../include
LINKS= -lpthread -lrt -L../../../../build/ -lrps

############################## PROFILE ###############################
TEST_SUITE = HA_PROFILE

############################## HA DEVICES ####################################
SUB_SUITE = ON_OFF_SWITCH

API_PATH = ../../../../
RSI_DRIVER_PATH = ../../../../include
ZB_APPLICATION_PATH = ./
ZB_API_TEST_PATH = ../examples/zigb/zb_test
ZB_INCLUDE = $(API_PATH)/zigbee/profiles/include
ZB_CORE_SRC = $(API_PATH)/driver/
ZB_REF_APPS_SRC=$(API_PATH)/zigbee/


DEFINES= $(INCLUDES) $(DEFS) 
CFLAGS= -g -Wall $(DEFINES) -Os
CFLAGS+= -D RSI_UART_INTERFACE
#debug prints are valid in uart case only
CFLAGS+= -D RSI_ENABLE_DEBUG_PRINT
CFLAGS+= -D RSI_SAMPLE_HAL
CFLAGS+= -D LINUX_PLATFORM
CFLAGS += -D ZIGBEE_END_DEVICE
#CFLAGS += -D ZIGBEE_COORDINATOR
#CFLAGS += -D ZIGBEE_ROUTER

COMMON_FLAGS+= -D RSI_ZB_ENABLE
COMMON_FLAGS+= -D HA_SA
COMMON_FLAGS+= -D RSI_SAMPLE_HAL
COMMON_FLAGS+= -D ZB_PROFILE

CFLAGS+= -DRSI_ZB_ENABLE
CFLAGS+= -DHA_SA
CFLAGS+=  -I$(ZB_APPLICATION_PATH)/
CFLAGS+=  -I$(ZB_INCLUDE)/
CFLAGS+=  -I$(RSI_DRIVER_PATH)/
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/include
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/utilities/stack_configuration/inc
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/utilities/Timer/inc
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/utilities/buffer_management/inc
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/utilities/queue_management/inc
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/zcl/ZCL_Interface/ 
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/zcl/ZCL_Foundation/inc/ 
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/zcl/ZCL_Common/inc/ 
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/zcl/ZCL_General/inc/ 
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/zcl/ZCL_SE/inc/ 
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/utilities/app_profile_utilities/inc/
CFLAGS+=  -I $(ZB_REF_APPS_SRC)/profiles/devices/ha_devices/OnOffSwitch/inc
CFLAGS+= -DZB_DEBUG
CFLAGS+= -DZB_PROFILE

ifeq ($(TEST_SUITE), HA_PROFILE)
CFLAGS+= -D g_APP_PROFILE_ID_c=0x0104
endif

ifeq ($(TEST_SUITE), SE_PROFILE)
CFLAGS+= -D g_APP_PROFILE_ID_c=0x0109
endif

ifeq ($(TEST_SUITE), ZLL_PROFILE)
CFLAGS+= -D g_APP_PROFILE_ID_c=0xc05e
endif

ZB_OBJS = $(ZB_CORE_SRC)/rsi_zigb.o \
					$(ZB_CORE_SRC)/rsi_zigb_utility.o \
				  $(ZB_REF_APPS_SRC)/rsi_zigb_apis.o  \


ZB_SWITCH_APP = rsi_switch.o \
								rsi_framework.o \
								rsi_app_main.o



all: $(ZB_SWITCH_APP) 

	make zb CC=$(CC) -C ../../../../build/ 
	make CC=$(CC) COMMON_FLAGS="$(COMMON_FLAGS)" TEST_SUITE=$(TEST_SUITE) SUB_SUITE=$(SUB_SUITE)  -C ../../../../zigbee/profiles/build/
	cp ../../../../zigbee/profiles/build/*.o ./
	$(CC) $(CFLAGS) *.o $(LINKS) -o $(PROGNAME) 
	rm -f  *.o  


clean:
	$(rm) $(ZB_SWITCH_APP) core *.o $(PROGNAME)*~
