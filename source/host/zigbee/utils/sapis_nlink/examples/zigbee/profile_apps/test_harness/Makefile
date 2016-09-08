##----------------------------------------------------------------------------
# Created with genmake.pl v1.1 on Thu Aug 20 16:58:17 2015
# genmake.pl home: http://muquit.com/muquit/software/
# Copryright: GNU GPL (http://www.gnu.org/copyleft/gpl.html)
##----------------------------------------------------------------------------
rm=/bin/rm -f
CC= cc
zb= zb
DEFS=  
PROGNAME= rsi_wc_app
INCLUDES=  -I../include
LINKS= -lpthread -lrt -L../../../../build/ -lrps

################### PROFILE SELECTION ##################
#TEST_SUITE = HA_PROFILE
#TEST_SUITE = SE_PROFILE
TEST_SUITE = ZLL_PROFILE

################### HA DEVICE SELECTION ##################

#SUB_SUITE = COLOR_DIMMABLE_LIGHT
#SUB_SUITE = COLOR_DIMMER_SWITCH
#SUB_SUITE = COMBINED_INTERFACE
#SUB_SUITE = CONFIGURATION_TOOL
#SUB_SUITE = CONSUMPTION_AWARENESS
#SUB_SUITE = DIMMABLE_LIGHT
#SUB_SUITE = DIMMER_SWITCH
#SUB_SUITE = DOOR_LOCK
#SUB_SUITE = DOOR_LOCK_CONTROLLER
#SUB_SUITE = FLOW_SENSOR
#SUB_SUITE = HEATING_COOLING_UNIT
#SUB_SUITE = HOME_GATEWAY
#SUB_SUITE = IAS_ACE
#SUB_SUITE = IAS_CIE
#SUB_SUITE = IAS_WD
#SUB_SUITE = IAS_ZONE
#SUB_SUITE = LEVEL_CONTROLLABLE_OUTPUT
#SUB_SUITE = LEVEL_CONTROL_SWITCH
#SUB_SUITE = LIGHT_SENSOR
#SUB_SUITE = MAINS_POWER_OUTLET
#SUB_SUITE = METER_INTERFACE
#SUB_SUITE = OCCUPANCY_SENSOR
#SUB_SUITE = ON_OFF_LIGHT
#SUB_SUITE = ON_OFF_LIGHT_SWITCH
#SUB_SUITE = ON_OFF_OUTPUT
#SUB_SUITE = ON_OFF_SWITCH
#SUB_SUITE = PRESSURE_SENSOR
#SUB_SUITE = PUMP
#SUB_SUITE = PUMP_CONTROLLER
#SUB_SUITE = RANGE_EXTENDER
#SUB_SUITE = REMOTE_CONTROL
#SUB_SUITE = SCENE_SELECTOR
#SUB_SUITE = SHADE
#SUB_SUITE = SHADE_CONTROLLER
#SUB_SUITE = SIMPLE_SENSOR
#SUB_SUITE = SMART_PLUG
#SUB_SUITE = TEMPERATURE_SENSOR
#SUB_SUITE = THERMOSTAT
#SUB_SUITE = WHITE_GOODS
#SUB_SUITE = WINDOW_COVERING
#SUB_SUITE = WINDOW_COVERING_CONTROLLER

############################## SE ###############################

#SUB_SUITE = ESP
#SUB_SUITE = IPD
#SUB_SUITE = LCD
#SUB_SUITE = METERING
#SUB_SUITE = PCT
#SUB_SUITE = PHYSICAL_DEVICE
#SUB_SUITE = PTD
#SUB_SUITE = RE
#SUB_SUITE = SA
#SUB_SUITE = ESP1
############################## SE ###############################

##############################ZLL################################
SUB_SUITE = CONTROLLER
#SUB_SUITE = LIGHT

API_PATH = ../../../../
RSI_DRIVER_PATH = ../../../../include
ZB_APPLICATION_PATH = ./
ZB_API_TEST_PATH = ../examples/zigb/zb_test
ZB_INCLUDE = $(API_PATH)/zigbee/profiles/include
ZB_CORE_SRC = $(API_PATH)/driver/
ZB_REF_APPS_SRC=$(API_PATH)/zigbee/


DEFINES= $(INCLUDES) $(DEFS) 
CFLAGS= -g -Wall $(DEFINES) -Os
CFLAGS+= -D UART_INTERFACE
CFLAGS+= -D DEBUG_DUMP_BYTES
#debug prints are valid in uart case only
CFLAGS+= -D RSI_ENABLE_DEBUG_PRINT
CFLAGS+= -D RSI_SAMPLE_HAL
CFLAGS+= -D LINUX_PLATFORM
CFLAGS+= -D ZB_DEBUG

COMMON_FLAGS+= -D RSI_ZB_ENABLE
COMMON_FLAGS+= -D HA_SA
COMMON_FLAGS+= -D RSI_SAMPLE_HAL
COMMON_FLAGS+= -D ZB_PROFILE

CFLAGS+= -DRSI_ZB_ENABLE
#CFLAGS+= -DHA_SA
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


ZB_SWITCH_APP = rsi_app_main.o



all: clean $(ZB_SWITCH_APP) 

	make zb CC=$(CC) -C ../../../../build/ 
	make CC=$(CC) COMMON_FLAGS="$(COMMON_FLAGS)" TEST_SUITE=$(TEST_SUITE) SUB_SUITE=$(SUB_SUITE)  -C ../../../../zigbee/profiles/build/
	cp ../../../../zigbee/profiles/build/*.o ./
	$(CC) $(CFLAGS) *.o $(LINKS) -o $(PROGNAME) 
	rm -f  *.o  


clean:
	make clean -C ../../../../zigbee/profiles/build/
	make clean -C ../../../../build/ 
	find $(APP_PATH) -iname *.o | xargs rm -rf
	$(rm) $(ZB_SWITCH_APP) core *.o $(PROGNAME)*~
