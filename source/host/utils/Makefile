#/**
# * @file Makefile 
# * @author 
# * @version 1.0
# *
# * @section LICENSE
# *
# * This software embodies materials and concepts that are confidential to Redpine
# * Signals and is made available solely pursuant to the terms of a written license
# * agreement with Redpine Signals
# *
# * @section DESCRIPTION
# *
# * This is the Utils Makefile used for generating the onebox utility. This utility is used for
# * creating the Virtual access point(VAP), deleting the VAP, setting wmm params etc
# */
#CC=$(CROSS_COMPILE)cc
cpy=cp

ifndef ROOT_DIR
ROOT_DIR=$(PWD)
endif


include $(ROOT_DIR)/.config
include $(ROOT_DIR)/config/make.config

#EXTRA_CFLAGS += -DENABLE_PER_MODE
EXTRA_CFLAGS += -MMD -O2 -Wall -g

OBJS =onebox_util.o

all: onebox_util copy
onebox_util: $(OBJS)
	$(CC) -o onebox_util $(OBJS)
copy:
	$(CC) -o receive receive.c
	$(CC) -o transmit transmit.c
	$(CC) -o transmit_packet transmit_packet.c
	#$(CC) -o nl80211_util nl80211_util.c
	$(CC) -o bbp_util matlab_utils.c
	$(CC) -o sniffer_app sniffer.c
	$(CC) -o zb_transmit zigb/zb_transmit.c zigb/rsi_nl_app.c
	$(CC) -o zb_util zigb/zb_utils.c zigb/rsi_nl_app.c
	$(CC) -o zb_bbp_util zigb/matlab_utils.c zigb/rsi_nl_app.c
	$(CC) -o bt_util bt_utils/bt_util.c bt_utils/rsi_nl_app.c -DCHIP_9116
	$(CC) -o bt_transmit bt_utils/bt_transmit.c bt_utils/rsi_nl_app.c
#	$(CC) -o bt_ber bt_utils/bt_ber.c bt_utils/rsi_nl_app.c
#	$(CC) -o bt_stats bt_utils/bt_stats.c bt_utils/rsi_nl_app.c
	$(CC) -o bt_bbp_utils bt_utils/matlab_utils.c bt_utils/rsi_nl_app.c
	$(CC) -o bt_receive bt_utils/bt_receive.c bt_utils/rsi_nl_app.c
	@echo -e "\033[32mcopying to release folder...\033[0m"
	$(cpy) onebox_util transmit transmit_packet receive bt_receive bbp_util sniffer_app bt_util bt_transmit bt_bbp_utils zb_transmit zb_util zb_bbp_util ../release/

clean:
	rm -f onebox_util onebox_util.d onebox_util.o
	rm -rf onebox_util transmit transmit_packet receive bt_receive bbp_util sniffer_app bt_util bt_freq_hop bt_ber bt_stats bt_transmit bt_bbp_utils zb_transmit zb_util zb_bbp_util
