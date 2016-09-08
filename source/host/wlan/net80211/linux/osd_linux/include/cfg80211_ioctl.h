#ifndef __CFG80211_IOCTL_H
#define __CFG80211_IOCTL_H

#define ONEBOX_STATUS_FAILURE -1
#define ONEBOX_STATUS_SUCCESS 0

#define	IFM_OMASK	0x0000ff00	/* Type specific options */
#define	IFM_MMASK	0x00070000	/* Mode */

struct wiphy;
struct ieee80211com;
struct ieee80211vap;
struct ieee80211_scan_entry;
struct station_info;


struct security_data {
uint32_t wpa_versions;
uint32_t cipher_group;
int n_ciphers_pairwise;
uint32_t ciphers_pairwise[5];
int n_akm_suites;
uint32_t akm_suites[2];
bool control_port;
__be16 control_port_ethertype;
bool control_port_no_encrypt;
};

struct obm_cfg_ap_beacon_ie {

    const uint8_t *head, *tail;
    const uint8_t *beacon_ies;
    const uint8_t *proberesp_ies;
    const uint8_t *assocresp_ies;
    const uint8_t *probe_resp;

    size_t head_len, tail_len;
    size_t beacon_ies_len;
    size_t proberesp_ies_len;
    size_t assocresp_ies_len;
    size_t probe_resp_len;
};

struct cfg80211_chan_config {

	uint8_t width;
	uint32_t center_freq_1;
	uint32_t center_freq_2;
	uint8_t band;
	uint16_t center_freq;
	uint32_t flags;
	uint8_t dfs_state;
	unsigned long dfs_state_entered;
};
struct obm_cfg80211_ap_params {
  
    uint8_t channel;
    uint8_t channel_width;
    /*cfg80211_beacon_data */
    const uint8_t *head, *tail;
    const uint8_t *beacon_ies;
    const uint8_t *proberesp_ies;
    const uint8_t *assocresp_ies;
    const uint8_t *probe_resp;

    size_t head_len, tail_len;
    size_t beacon_ies_len;
    size_t proberesp_ies_len;
    size_t assocresp_ies_len;
    size_t probe_resp_len;
    /* End of cfg80211_beacon_data */

    uint32_t beacon_interval, dtim_period;
    const uint8_t *ssid;
    uint8_t ssid_len;

    uint8_t auth_mode;
    /**Security parameters*/
    struct security_data security;
    bool privacy;
    bool hide_ssid;

	struct cfg80211_chan_config chan;
};
#define IEEE80211_ADDR_LEN 6
#define IEEE80211_NWID_LEN 32

struct ieee80211_scan_h {
	uint8_t		se_macaddr[IEEE80211_ADDR_LEN];
	uint8_t		se_bssid[IEEE80211_ADDR_LEN];
	uint8_t		se_ssid[2+IEEE80211_NWID_LEN];
	uint16_t	se_intval;	/* beacon interval (host byte order) */
	uint16_t	se_capinfo;	/* capabilities (host byte order) */
	struct ieee80211_channel *se_chan;/* channel where sta found */
	uint8_t	ic_ieee;	/* IEEE channel number */
	uint8_t	*data;		/* frame data > 802.11 header */
	int	len;		/* data size in bytes */
	int8_t		se_rssi;	/* avg'd recv ssi */
	int8_t		se_noise;	/* noise floor */
};

struct local_info {
	bool connected;
	u64 rx_bytes;
	u64 tx_bytes;
	u32 tx_data;
	u32 rx_data;
	s8 signal;
	u16 tx_legacy_rate;
	u16 rx_legacy_rate;
	bool wme_ie;
	int mode;
};
enum ieee80211_notify_cac_event;
enum nl80211_radar_event;

size_t os_nl_strlcpy(char *dest, const char *src, size_t siz);
void nl80211_mem_alloc(void **ptr, unsigned short len, unsigned short flags);
void* nl80211_memcp(void *to, const void *from, int len);
void* nl80211_memcpy(void *to, const void *from, int len);
int scan_results_sup(struct wireless_dev *wdev, struct ieee80211_scan_h *hscan);
int scan_done(void *temp_scan_req, bool scan_failed);
#if((LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) && (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 5, 7)))
void obm_inform_mgmt_cfg80211(struct net_device *dev , uint8_t *buf, uint16_t len, uint16_t recv_freq, int32_t sig_dbm);
#else
void obm_inform_mgmt_cfg80211(struct wireless_dev *wdev, uint8_t *buf, uint16_t len, uint16_t recv_freq, int32_t sig_dbm);
#endif
void obm_notify_cfg80211(struct net_device *dev, enum ieee80211_notify_cac_event cmd);
void obm_cfg80211_radar_notify( struct net_device *dev, enum nl80211_radar_event type, struct obm_cfg80211_ap_params *ap_params);

struct ieee80211_key_params
{
	uint8_t *key;
	uint8_t *seq;
	int key_len;
	int seq_len;
	uint32_t cipher;
};

#define IEEE80211_MAX_SSID_LEN 32
struct ieee80211_ssid
{
	uint8_t ssid[IEEE80211_MAX_SSID_LEN];
	uint8_t ssid_len;
};
/* cipher suite selectors */
#define WLAN_CIPHER_SUITE_USE_GROUP	0x000FAC00
#define WLAN_CIPHER_SUITE_WEP40		0x000FAC01
#define WLAN_CIPHER_SUITE_TKIP		0x000FAC02
/* reserved: 				0x000FAC03 */
#define WLAN_CIPHER_SUITE_CCMP		0x000FAC04
#define WLAN_CIPHER_SUITE_WEP104	0x000FAC05
#define WLAN_CIPHER_SUITE_AES_CMAC	0x000FAC06

#define WPA_PROTO_WPA BIT(0)
#define WPA_PROTO_RSN BIT(1)

#define WPA_AUTH_ALG_SHARED BIT(0)

#define BAND_WIDTH_20MHZ 1
#define BAND_WIDTH_40MHZ 2

enum wpa_alg {
	WPA_ALG_NONE,
	WPA_ALG_WEP,
	WPA_ALG_TKIP,
	WPA_ALG_CCMP,
	WPA_ALG_IGTK,
	WPA_ALG_PMK
};
typedef struct
{
	unsigned short timestamp[4];
	unsigned short beacon_intvl;
	unsigned short capability;
	unsigned char variable[0];
}__attribute__ ((packed)) BEACON_PROBE_FORMAT;

typedef struct
{
int auth_type;
const uint8_t *ie;
size_t ie_len;
const unsigned char *ssid;
size_t ssid_len;
unsigned char *bssid;
int len;
int privacy;
}cfg80211_sta_connect_params;

typedef struct 
{
	uint8_t addr[ETH_ALEN];
}acl_mac_address;

/* The below enum should be equal to ieee80211_channel_flags in cfg80211
 * Do not modify unnecessarily*/
enum cfg80211_channel_flags {
         CFG80211_CHAN_DISABLED         = 1<<24,
         CFG80211_CHAN_PASSIVE_SCAN     = 1<<1,
         CFG80211_CHAN_NO_IBSS          = 1<<2,
         CFG80211_CHAN_RADAR            = 1<<3,
         CFG80211_CHAN_NO_HT40PLUS      = 1<<4,
         CFG80211_CHAN_NO_HT40MINUS     = 1<<5,
};


#define LEGACY_INDEX_1     1
#define LEGACY_INDEX_2     2
#define LEGACY_INDEX_4     4
#define LEGACY_INDEX_8     8
#define LEGACY_INDEX_16    16
#define LEGACY_INDEX_32    32
#define LEGACY_INDEX_64    64
#define LEGACY_INDEX_128   128
#define LEGACY_INDEX_256   256
#define LEGACY_INDEX_512   512
#define LEGACY_INDEX_1024  1024
#define LEGACY_INDEX_2048  2048

#define cfg80211_CHAN_NO_HT40 \
        (cfg80211_CHAN_NO_HT40PLUS | cfg80211_CHAN_NO_HT40MINUS)


struct cfg80211_priv* cfg80211_wrapper_attach(struct device *dev, u8 *mac_addr , struct ieee80211com *ic, uint8_t dual_band);
void cfg80211_wrapper_detach(struct wiphy *wiphy);
struct wireless_dev* onebox_register_wireless_dev(struct net_device *ndev, struct cfg80211_priv *cfg_priv, int opmode);

int onebox_prepare_ioctl_cmd(struct ieee80211vap *vap, uint8_t type, const void *data, int val, int len );

uint8_t onebox_delete_key(struct net_device *ndev, const uint8_t *mac_addr, uint8_t index);

uint8_t  onebox_add_key(struct net_device *ndev, uint8_t index, 
                             const uint8_t *mac_addr,struct ieee80211_key_params *params );

uint8_t onebox_wep_key(struct net_device *ndev, int index, const uint8_t *mac_addr, uint8_t key_len, const uint8_t *key);
int32_t onebox_siwfrag(struct ieee80211com *ic, struct iw_param *frag);
int32_t onebox_siwrts(struct ieee80211com *ic, struct iw_param *rts);
uint8_t tx_power(struct ieee80211com *ic, int dbm);
int onebox_set_if_media(struct net_device *dev, int media);
int onebox_get_if_media(struct net_device *dev);
int onebox_set_mediaopt(struct net_device *dev, uint32_t mask, uint32_t mode);
int onebox_driver_nl80211_set_wpa(struct ieee80211vap *vap, int enabled);
int onebox_driver_nl80211_set_wpa_internal(struct ieee80211vap *vap, int wpa, int privacy);
int nl80211_ctrl_iface(struct net_device *ndev, int enable);
int cfg80211_connect_res(struct net_device *ndev, cfg80211_sta_connect_params *connect_params);
int cfg80211_inform_scan_results(void *arg, void *se_nl);
int cfg80211_disconnect(struct net_device *ndev, int reason_code);

void notify_sta_connect_to_cfg80211(struct wireless_dev *wdev, uint8_t mac[6]);
void onebox_inform_cfg80211_new_node_join(struct ieee80211vap *vap, uint8_t *mac_address, uint8_t *assoc_req_ie, uint8_t len);
void inform_cfg80211_node_join(struct net_device *ndev, uint8_t *mac_address, uint8_t *assoc_req_ie, uint8_t assoc_ie_len);
void inform_cfg80211_node_leave(struct net_device *ndev, uint8_t *mac_address);
void notify_sta_disconnect_to_cfg80211(struct wireless_dev *wdev);
int onebox_inform_bss_to_cfg80211(struct ieee80211_scan_entry *ise, struct ieee80211vap *vap);
int obm_delete_connected_sta(struct net_device *ndev, const uint8_t *mac,
                         uint8_t opcode, uint16_t reason_code);
void onebox_update_country(uint8_t *country, struct cfg80211_priv *cfg_priv);
void notify_mic_failure_to_cfg80211(struct net_device *ndev, 
                                    uint8_t *mac_addr, 
																		uint8_t key_type,
																		int32_t key_id);

#endif

int obm_dump_station_info(struct net_device *dev,struct local_info * data, u8 *mac,struct station_info *sinfo, int idx);
