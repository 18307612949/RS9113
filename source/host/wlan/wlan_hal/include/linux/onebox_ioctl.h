/**
 * @file onebox_ioctl.h
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
 * This file contians the ioctl related prototypes and macros
 *  
 */

#ifndef __ONEBOX_AP_IOCTL_H__
#define __ONEBOX_AP_IOCTL_H__

#define ONEBOX_VAP_CREATE        SIOCIWLASTPRIV - 0xf  //: check for exact value b4 execution
#define ONEBOX_VAP_DELETE        SIOCIWLASTPRIV - 0x10
#define ONEBOX_HOST_IOCTL        SIOCIWLASTPRIV - 0x0B
#define RSI_WATCH_IOCTL          SIOCIWLASTPRIV - 0xd 
#define SET_BEACON_INVL          18
#define SET_BGSCAN_PARAMS        19
#define DO_BGSCAN                20
#define BGSCAN_SSID              21
#define EEPROM_READ_IOCTL        22
#define EEPROM_WRITE_IOCTL       23
#define PS_REQUEST               25
#define UAPSD_REQ                26
#define RESET_ADAPTER            32
#define RESET_PER_Q_STATS        33 
#define AGGR_LIMIT               34
#define RX_FILTER		35
#define MASTER_READ		36
#define MASTER_WRITE		37
#define TEST_MODE		38
#define RF_PWR_MODE		39
#define SET_COUNTRY 40
#define GET_INFO 41
#define SET_SCAN_TYPE 42
#define SET_WOWLAN_CONFIG 43
#define SET_EXT_ANT_GAIN 44
#define ADD_APPLE_IE 46
#define IAP_INIT	47
#define IAP_MFI_CHALLENGE	48
#define IAP_READ_CERTIFICATE	49
#define DRV_PARAMS		50
#define CONF_BEACON_RECV		51

#define ONEBOX_SET_BB_RF         SIOCIWLASTPRIV - 0x08 
#define ONEBOX_SET_CW_MODE       SIOCIWLASTPRIV - 0x05 

/* To reset the per queue traffic stats */
//: Free ioctl num 0 , can be used.
//#define xxx      SIOCIWFIRSTPRIV + 0

#define SIOCGPROTOCOL                (SIOCIWFIRSTPRIV+21)

struct mfi_challenge {
  uint8 challenge_data[20];
};

bool check_valid_bgchannel(uint16 *data_ptr, uint8_t supported_band);
int ieee80211_ioctl_delete_vap(struct ieee80211com *ic, struct ifreq *ifr, struct net_device *mdev);
int ieee80211_ioctl_create_vap(struct ieee80211com *ic, struct ifreq *ifr, struct net_device *mdev);
int onebox_ioctl(struct net_device *dev,struct ifreq *ifr, int cmd);
typedef int (*ioctl_handler_t)(WLAN_ADAPTER w_adapter, struct iwreq *wrq);
#endif
