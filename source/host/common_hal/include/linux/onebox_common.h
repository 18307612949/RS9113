/**
 * @file onebox_common.h
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
 * This file contians the data structures and variables/ macros commonly
 * used in the driver .
 */

#ifndef __ONEBOX_COMMON_H__
#define __ONEBOX_COMMON_H__

#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/etherdevice.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/mutex.h>
#include <linux/usb.h>

/*Bluetooth Specific */
#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
#include <linux/workqueue.h>

#define onebox_likely(a) likely(a)
#define USB_VENDOR_REGISTER_READ 0x15
#define USB_VENDOR_REGISTER_WRITE 0x16
/***Dont USE 0x17**/
#define USB_VENDOR_PS_STATUS_READ 0x18

#if((LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18))&& \
    (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,23)))
#include <linux/sdio/ctsystem.h>
#include <linux/sdio/sdio_busdriver.h>
#include <linux/sdio/_sdio_defs.h>
#include <linux/sdio/sdio_lib.h>
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#endif
#include <linux/interrupt.h>

#include "onebox_datatypes.h"

 extern uint32 onebox_zone_enabled;
/* Kernel version between and including a & b */
#define KERNEL_VERSION_BTWN_2_6_(a,b) \
  ((LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,a)) && \
  (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,b)))

#define KERNEL_VERSION_EQUALS_2_6_(a) \
  (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,a))

/* Kernel version greater than equals */
#define KERNEL_VERSION_GREATER_THAN_2_6_(a)  \
 (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,a)) 

/* Kernel version less than or equal to */
#define KERNEL_VERSION_LESS_THAN_3_6(a)  \
 (LINUX_VERSION_CODE <= KERNEL_VERSION(3,6,a)) 

#define KERNEL_VERSION_LESS_THAN_3_12_(a) \
 (LINUX_VERSION_CODE <= KERNEL_VERSION(3,12,a)) 


#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif


#if 0 /* coex */
#ifdef ONEBOX_DEBUG
#define obm_dbg(__zone, __str, __arg...) \
	do { \
		if((!zone) || (zone & onebox_zone_enabled)) \
			printk(_str, __arg);
	} while(0)
#elif
#define obm_dbg(__arg) \
	((void*)0) 
#endif

#define ONEBOX_BIT(n)	(1 << (n))
#define Z_ERRO     ONEBOX_BIT(0)  /* For Error Msgs              */
#define Z_WARN     ONEBOX_BIT(1)  /* For Warning Msgs            */
#define Z_INFO     ONEBOX_BIT(2)  /* For General Status Msgs     */
#define Z_INIT     ONEBOX_BIT(3)  /* For Driver Init Seq Msgs    */
#define Z_OID      ONEBOX_BIT(4)  /* For IOCTL Path Msgs         */
#define Z_MGTX     ONEBOX_BIT(5)  /* For TX Mgmt Path Msgs       */
#define Z_MGRX
#define Z_DATX
#define Z_DARX
#define Z_FSM      ONEBOX_BIT(9)  /* For State Machine Msgs      */
#define Z_ISR      ONEBOX_BIT(10) /* For Interrupt Specific Msgs */
#define Z_MGDMP    ONEBOX_BIT(11) /* For Dumping Mgmt Pkts       */
#define Z_DADMP    ONEBOX_BIT(12) /* For Dumping Data Pkts       */
#define Z_CLAS     ONEBOX_BIT(13) /* For Classification Msgs     */
#define Z_PROB     ONEBOX_BIT(14) /* For Probe Req & Rsp Msgs    */
#define Z_EXIT     ONEBOX_BIT(15) /* For Driver Unloading Msgs   */
#define Z_DBUG     ONEBOX_BIT(16) /* For Extra Debug Messages    */
#define Z_PWRSV    ONEBOX_BIT(17) /* For Powersave Blk Msgs      */
#define Z_AGGR     ONEBOX_BIT(18) /* For Aggregation Msgs        */
#define Z_DAGGR    ONEBOX_BIT(19) /* For Deaggregation Msgs      */
#define Z_ARATE    ONEBOX_BIT(20) /* For Autorate Msgs           */

#define OB_SUCCESS       0
#define OB_FAILURE      (!0)
#endif

#define INVALID_QUEUE              0xff
#define NO_STA_DATA_QUEUES          4

#define ONEBOX_HEADER_SIZE          18 /* Size of PS_POLL */
#define TX_VECTOR_SZ                12

#define ONEBOX_TOTAL_PWR_VALS_LEN   30
#define ONEBOX_BGN_PWR_VALS_LEN     30
#define ONEBOX_AN_PWR_VALS_LEN      18

#define ONEBOX_EXTERN   extern
#define ONEBOX_STATIC   static
#define HEX_FILE       1
#define BIN_FILE       0
#define ONEBOX_SDIO_FRM_TRF_SIZE   (256 - 16)
#define WIFI_MODE_ON    1
#define RF_EVAL_MODE_ON    2
#define RF_EVAL_LPBK_CALIB 3
#define RF_EVAL_LPBK       4
#define QSPI_FLASHING   5
#define QSPI_UPDATE     6
#define SNIFFER_MODE    7
#define SWBL_FLASHING_NOSBL   8
#define SWBL_FLASHING_SBL     9
#define SWBL_FLASHING_NOSBL_FILE   10

#define UNIX_FILE_TYPE  8
#define DOS_FILE_TYPE   9

#define MAX_NUM_OF_SETS            15
#define  MAX_NUM_OF_BBP_VALS_PER_SET  250

/******* COEX MODE *************/
#define WIFI_ALONE 1
#define WIFI_BT_CLASSIC 2
#define WIFI_ZIGBEE 3
#define WIFI_BT_LE 4
/*******************************/

/********OPER_MODE****************/
#define OP_WLAN_STA_MODE	BIT(0)
#define OP_WLAN_AP_MODE		BIT(1)
#define OP_BT_CLASSIC_MODE	BIT(2)
#define OP_BT_LE_MODE		BIT(3)
#define OP_ZIGBEE_MODE		(BIT(4) | BIT(5))
/*******************************/

/**** ZigBee Operating Modes ****/
#define ZIGBEE_END_DEVICE    1
#define ZIGBEE_COORDINATOR   2
#define ZIGBEE_ROUTER        3
/*******************************/

#define ZIGBEE_OPERMODE_MASK 0x3

//#define WIFI_STA 1
//#define WIFI_AP 2
/*******************************/

typedef void (*SD_INTERRUPT) (void *pContext);

typedef int ONEBOX_STATUS;


#ifndef DANUBE_ADDRESSING
 #define ONEBOX_HOST_VIR_TO_PHY(virt_addr)  virt_to_phys((void *)virt_addr)
 #define ONEBOX_HOST_PHY_TO_VIR(phy_addr)  phys_to_virt((uint32)phy_addr)
#else
 #define ONEBOX_HOST_VIR_TO_PHY(virt_addr) ((void *)(((uint32)virt_addr) & 0x5FFFFFFF))
 #define ONEBOX_HOST_PHY_TO_VIR(phy_addr) (((uint32)phy_addr) | 0xA0000000)
#endif

#define ONEBOX_CPU_TO_LE16(x)   cpu_to_le16(x)
#define ONEBOX_CPU_TO_LE32(x)   cpu_to_le32(x)
#define ONEBOX_CPU_TO_BE16(x)   cpu_to_be16(x)

#define COEX_SOFT_QUEUES  5 

/*Master access types*/
#define ONEBOX_MASTER_READ	11
#define ONEBOX_MASTER_WRITE	22
#define ONEBOX_MASTER_ACK	33

#define MISC_REG	     0x41050012
#define ROM_PC_RESET_ADDRESS 0x25000

/*Firmware loading modes*/
#define FULL_FLASH_SBL		1
#define FULL_RAM_SBL		2
#define FLASH_RAM_SBL		3
#define FLASH_RAM_NO_SBL	4

#define FLASH_SECTOR_SIZE (4 * 1024)
#define STARTING_BLOCK_INDEX 0
#define FLASH_BLOCK_SIZE (32 * 1024)

#define EEPROM_VERSION_OFFSET 77
#define CALIB_CRC_OFFSET      4092
#define MAGIC_WORD 0x5A
#define MAGIC_WORD_OFFSET_1 40
#define MAGIC_WORD_OFFSET_2 424
#define FW_IMAGE_MIN_ADDRESS 68 * 1024
#define FLASH_MAX_ADDRESS       4 * 1024 * 1024 //4MB
#define MAX_FLASH_FILE_SIZE	400 * 1024 //400K
#define FLASHING_START_ADDRESS  16
#define CALIB_VALUES_START_ADDR 16
#define SOC_FLASH_ADDR  0x04000000
#define EEPROM_DATA_SIZE 4096
#define CALIB_DATA_SIZE (EEPROM_DATA_SIZE - CALIB_VALUES_START_ADDR)
#define BL_HEADER 32

#define FLASH_SIZE_ADDR     0x04000016
#define PING_BUFFER_ADDRESS 0x19000
#define PONG_BUFFER_ADDRESS 0x1a000
#define SWBL_REGIN	    0x41050034
#define SWBL_REGOUT	    0x4105003c
#define PING_WRITE	    0x1
#define PONG_WRITE	    0x2

#define REGIN_VALID 0xA
#define REGIN_INPUT (0xA0 | adapter->coex_mode)
#define REGOUT_VALID 0xAB
#define REGOUT_INVALID (~0xAB)
#define CMD_PASS 0xAA
#define CMD_FAIL 0xCC
#define INVALID_ADDR 0x4C
#define SEND_RPS_FILE '2'

#define BURN_BL 0x23//'#' /*0x23*/
#define LOAD_HOSTED_FW 'A'
#define BURN_HOSTED_FW 'B'
#define PING_VALID 'I'
#define PONG_VALID 'O'
#define PING_AVAIL 'I'
#define PONG_AVAIL 'O'
#define EOF_REACHED 'E'
#define CHECK_CRC 'K'
#define POLLING_MODE 'P'
#define CONFIG_AUTO_READ_MODE 'R'
#define JUMP_TO_ZERO_PC 'J'
#define FW_LOADING_SUCCESSFUL 'S'
#define LOADING_INITIATED '1'

#define BL_CMD_TIMEOUT 2000
#define BL_BURN_TIMEOUT 30 * 1000

#define MASTER_READ_MODE 1
#define EEPROM_READ_MODE 2

#define FW_FLASH_OFFSET                 0x820
#define LMAC_VER_OFFSET                 FW_FLASH_OFFSET +0x200
#define COMMON_PKT_LENGTH         2304 // 1536

/* Sniffer mode */
#define MAX_SNIFFER_MODE_PKT_LEN  4096

#define GPIO_HIGH 1
#define GPIO_LOW 0
struct ta_metadata
{
	char *name;
	unsigned int address;
};
typedef struct ta_metadata METADATA;

typedef struct
{
	uint16  major;
	uint16  minor;
	uint8 release_num;
	uint8 patch_num;
	union 
	{
		struct
		{
			uint8 fw_ver[8];
		}info;
	}ver;
}__attribute__ ((packed))version_st;

/* RF Types */
typedef enum  
{ 
	ONEBOX_RF_8230 = 0,
	ONEBOX_RF_8111 
}rf_type;//core related


//! structure to store configs related to TAPLL programming
typedef struct tapll_info_s
{
   //! configs for pll reg 1
   uint32 pll_config_register_1 : 16;
   //! configs for pll reg 2
   uint32 pll_config_register_2 : 16; 
}__attribute__((__packed__)) tapll_info_t;

//! structure to store configs related to PLL960 programming
typedef struct pll960_info_s
{
   //! configs for pll reg 1
   uint32 pll_config_register_1 : 16;
   //! configs for pll reg 2
   uint32 pll_config_register_2 : 16; 
   //! configs for pll reg 3
   uint32 pll_config_register_3 : 16;
}__attribute__((__packed__)) pll960_info_t;

//! structure to store configs related to AFEPLL programming
typedef struct afepll_info_s
{
   //! configs for pll reg 
   uint32 pll_config_register : 16;
}__attribute__((__packed__)) afepll_info_t;


/* structure to store configs related to pll configs */
typedef struct pll_config_s {
  //! tapll info is a daughter structure
  tapll_info_t tapll_info_g;
  //! pll960 info is a daughter structure
  pll960_info_t pll960_info_g;
  //! afepll info is a daughter structure
  afepll_info_t afepll_info_g;
}__attribute__((__packed__)) pll_config_t;

//! structure to store configs related to UMAC clk programming
typedef struct switch_clk_s
{
  //! If set rest is valid
  uint32 switch_umac_clk : 1;
  //! If set qspi clk will be changed
  uint32 switch_qspi_clk : 1;
  //! If set sleep clk will be changed to 32MHz
  uint32 switch_slp_clk_2_32 : 1;
  //! If set program the bbp lmac clock reg val into the BBP_LMAC_CLOCK_REG
  uint32 switch_bbp_lmac_clk_reg : 1;
  //! If set config mem_ctrl for 120Mhz support
  uint32 switch_mem_ctrl_cfg : 1;
  //! reserved
  uint32 reserved : 11;
  //! If switch_bbp_lmac_clk_reg is set then this value will be programmed into reg
  uint32 bbp_lmac_clk_reg_val : 16;
  //! if switch_umac_clk is set then this value will be programmed to reg
  uint32 umac_clock_reg_config : 16;
  //! if switch_qspi_clk is set then this value will be programmed to reg
  uint32 qspi_uart_clock_reg_config : 16;
} switch_clk_t;


typedef struct device_clk_info_s
{
  pll_config_t pll_config_g;
  switch_clk_t switch_clk_g;
} device_clk_info_t;

/* structure contains bootup params use for configuring device */
typedef struct bootup_params
{
  /* indicates whether the params are loaded by external source or by bootloading*/
  uint16 magic_number;
#define LOADED_TOKEN  0x5AA5   /* Bootup params are installed by host or OTP/FLASH (Bootloader) */
#define ROM_TOKEN     0x55AA   /* Bootup params are taken from ROM itself in MCU mode. */
#if 0 /* Add proper comments for the below fields */
  uint16 internal_pmu_enabled;  
  uint16 ldo_control_required;   
  uint16 crystal_frequency;      
  uint16 sleep_clock_source;
  uint16 voltage_control_enabled;
  uint16 wakeup_threshold_time;
  uint16 m_ratio;
  uint16 input_d_ratio;
  uint16 output_d_ratio;
  uint16 host_wakeup_enable;
  uint16 host_interrupt_enable;
  uint16 host_wakeup_active_high;
  uint16 xtal_ip_enable;
  uint16 external_pa;
#endif
  uint16 crystal_good_time;
  uint32 valid;
#define CRYSTAL_GOOD_TIME                BIT(0)
#define BOOTUP_MODE_INFO                 BIT(1) 
#define DIGITAL_LOOP_BACK_PARAMS         BIT(2)
#define RTLS_TIMESTAMP_EN                BIT(3) 
#define HOST_SPI_INTR_CFG                BIT(4)
#define WIFI_TAPLL_CONFIGS               BIT(5)
#define WIFI_PLL960_CONFIGS              BIT(6)
#define WIFI_AFEPLL_CONFIGS              BIT(7)
#define WIFI_SWITCH_CLK_CONFIGS          BIT(8) 
#define BT_TAPLL_CONFIGS                 BIT(9)
#define BT_PLL960_CONFIGS                BIT(10)
#define BT_AFEPLL_CONFIGS                BIT(11)
#define BT_SWITCH_CLK_CONFIGS            BIT(12)
#define ZB_TAPLL_CONFIGS                 BIT(13)
#define ZB_PLL960_CONFIGS                BIT(14)
#define ZB_AFEPLL_CONFIGS                BIT(15)
#define ZB_SWITCH_CLK_CONFIGS            BIT(16)
#define BUCKBOOST_WAIT_INFO              BIT(17)
#define PMU_WAKEUP_SHUTDOWN_W            BIT(18)
#define WDT_PROG_VALUES                  BIT(19)
#define WDT_RESET_DELAY_VALUE            BIT(20)
#define DCDC_OPERATION_MODE_VALID        BIT(21) 
#define PMU_SLP_CLKOUT_SEL               BIT(22)
#define SOC_RESET_WAIT_CNT               BIT(23)
  //! reserved for future use            
  uint32 reserved_for_valids;
  //! Add proper comment
  uint16 bootup_mode_info; 
#define BT_COEXIST                       BIT(0)
#define BOOTUP_MODE                     (BIT(2) |BIT(1))
#define CUR_DEV_MODE                    (bootup_params.bootup_mode_info >> 1)
  //! configuration used for digital loop back
  uint16 digital_loop_back_params;
  //! Add proper comment
  uint16 rtls_timestamp_en;
  //! reserved
  uint16 host_spi_intr_cfg;
#if 0
  pll_config_t wifi_pll_config_g;
  switch_clk_t wifi_switch_clk_g;
  pll_config_t bt_pll_config_g;
  switch_clk_t bt_switch_clk_g;
  pll_config_t zb_pll_config_g;
  switch_clk_t zb_switch_clk_g;
#endif
  device_clk_info_t device_clk_info[3];
  //! ulp buckboost wait time 
  uint32 buckboost_wakeup_cnt;
  //! pmu wakeup wait time & WDT EN info
  uint16 pmu_wakeup_wait;
  //! ulp shutdown wait time 
  uint8 shutdown_wait_time;
  //! Sleep clock source selection 
  uint8 pmu_slp_clkout_sel;
  //! WDT programming values
  uint32 wdt_prog_value;
  //! WDT soc reset delay
  uint32 wdt_soc_rst_delay;
  //! dcdc modes configs
  uint32 dcdc_operation_mode;
  //! update doc and make changes here, this is not in proper position
  uint32 soc_reset_wait_cnt;
  uint32 waiting_time_at_fresh_sleep;
  uint32 max_threshold_to_avoid_sleep;
  uint8 beacon_resedue_alg_en;
}BOOTUP_PARAMETERS;


typedef struct config_vals
{
	uint16 internalPmuEnabled;  // 1 or 0
	uint16 ldoControlRequired;  // 1 or 0 
	uint16 crystalFrequency;      //0  9.6MHz, 1  19.2MHz, 2  38.4MHz, 4  13MHz
	                                             //5  26MHz, 6  52MHz, 8  20MHz, 9  40MHz
	uint16 crystalGoodTime;
	uint16 TAPLLEnabled;  // 1 or 0
	uint16 TAPLLFrequency;
	uint16 sleepClockSource;
	uint16 voltageControlEnabled;
	uint16 wakeupThresholdTime;
	uint16 mratio;
	uint16 inputdratio;
	uint16 outputdratio;
	uint16 host_wkup_enable;
	uint16 fr4_enable; 
	uint16 BT_coexistence;
	uint16 host_wakeup_enable;
	uint16 host_wakeup_active_high;
	uint16 Xtal_ip_enable;
	uint16 ExternalPA;
}CONFIG_VALS,*PCONFIG_VALS,BOOTUP_PARAMETERS_OLD;
#define MAC_ADDR_LEN        6


typedef struct onebox_priv ONEBOX_ADAPTER, *PONEBOX_ADAPTER;

#include "onebox_netbuf.h"
#include "onebox_coex.h"
#include "onebox_mgmt.h"
#include "onebox_hal_ops.h"
#include "onebox_os_intf_ops.h"
#include "onebox_eeprom.h"

typedef struct urb_context_s {
	PONEBOX_ADAPTER adapter;
	netbuf_ctrl_block_t *netbuf_addr;
	uint8 ep_num;
	uint8 urb_in_use;
} urb_context_t;

/* Adapter structure */
struct onebox_priv 
{
	onebox_thread_handle_t    sdio_scheduler_thread_handle;

	uint32            TransmitBlockSize;
	uint32            ReceiveBlockSize;
	uint32            CardCapability;
	uint8             *DataRcvPacket;
	struct net_device_stats  stats;

	uint8       mac_addr[6];
	/*version related variables*/
	version_st        driver_ver;
	version_st        ta_ver;
	version_st        lmac_ver;

	onebox_netbuf_head_t     coex_queues[COEX_SOFT_QUEUES]; 

#define NAME_MAX_LENGTH         32
	uint8               name[NAME_MAX_LENGTH];
	
	uint32        sdio_intr_status_zero;
	uint32	      tot_pkts_qed[COEX_SOFT_QUEUES];
	uint32	      tot_pkts_sentout[COEX_SOFT_QUEUES];
	uint32	      tot_pkts_dropped[COEX_SOFT_QUEUES];

	uint32        sdio_int_counter;
	uint32        total_sdio_msdu_pending_intr;
	uint32        total_sdio_unknown_intr;
	uint32        buf_status_interrupts;
	uint32        total_dma_mem_allocated;
	uint32        total_mem_allocated;
	uint32        interrupt_status;

	unsigned char   operating_chwidth;
	uint8         ShutDown;

	rf_type       RFType;

	uint32        next_read_delay;
	uint32        sdio_high_speed_enable;

#define ONEBOX_USB_TX_HEAD_ROOM 128
	struct usb_device *usbdev;
	struct usb_interface *usb_pfunction;
#define MAX_BULK_EP 8
	uint32 bulk_in_size[MAX_BULK_EP];//The size of recv buffer
	uint8 bulk_in_endpointAddr[MAX_BULK_EP];//The address of bulk in EP
	/* the size of the send buffer */
	uint32 bulk_out_size[MAX_BULK_EP];
	/* the address of the bulk out endpoint */
	uint8  bulk_out_endpointAddr[MAX_BULK_EP];
	uint32 total_usb_rx_urbs_submitted;
	uint32 total_usb_tx_urbs_submitted[MAX_BULK_EP];
	uint32 total_usb_rx_urbs_done;
	uint32 total_usb_tx_urbs_done[MAX_BULK_EP];
#define MAX_RX_URBS 5
#define MAX_TX_URB 16
	struct urb *rx_usb_urb[MAX_RX_URBS];
	struct urb *tx_usb_urb[MAX_TX_URB];
	struct urb *tx_coex_usb_urb[MAX_RX_URBS];
	urb_context_t *context_urb[MAX_TX_URB];
	atomic_t tx_pending_urb_cnt;
	uint8 write_urb;
	uint8 read_urb;
	struct usb_sg_request usb_sg;
	//void*  usb_dev_sem;
	ONEBOX_EVENT usb_rx_event;
	ONEBOX_EVENT usb_tx_event;
#if((LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18))&& \
    (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,23)))
	PSDDEVICE pDevice;
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
	
	struct  sdio_func *sdio_pfunction;
	struct task_struct    *in_sdio_litefi_irq;
#endif

#ifdef ONEBOX_CONFIG_CFG80211
	struct device *device;
#endif

	struct onebox_os_intf_operations *os_intf_ops;
	struct onebox_osd_host_intf_operations *osd_host_intf_ops;
	struct onebox_osi_host_intf_operations *osi_host_intf_ops;
	struct onebox_coex_osi_operations *coex_osi_ops;

	struct mutex sdio_interrupt_lock;
	spinlock_t master_ops_lock ;
	struct semaphore transmit_lock;
	uint8  firmware_path[256];

	uint8           Driver_Mode;
	uint8		flashing_mode;
	uint8           eeprom_erase;
	uint8           eeprom_init;
	uint8		eeprom_cfm_count;
	EEPROMRW        eeprom;
	uint32		flash_capacity;
	uint8 		*calib_data;
	     
	uint8           fw_load_mode;
	uint8           skip_fw_load; 
	uint32          fsm_state;

	/*godavari related */
	eepromrw         eepromrw_info;
	onebox_sensitivity sensitivity;
	uint16	sdio_clock_speed;
	bool	PassiveScanEnable;
	uint8	chk_sum_offld_cfg;	
	uint8	isMobileDevice;
	uint8	eeprom_address_type;
	uint8	PermanentAddress[6];
	bool	extPAenabled;
	uint8 stop_card_write;
	uint8 prev_desc[FRAME_DESC_SZ];
	uint32 			total_per_pkt_sent ;

	ONEBOX_EVENT    flash_event;
	uint8           qspi_flashing;
	uint8           flashing_started;
	uint8           flashing_mode_on;
	uint32          flash_offset;
	struct timer_list bl_timer;
	uint8 		bl_timer_expired;

#define ULP_SLEEP_ENTRY 1
	uint16 					ulp_sleep_token;

	unsigned long sdio_thread_counter ; //=0
#ifdef USE_WORKQUEUES
	struct workqueue_struct *int_work_queue; /* interrupt bottom half queue */
	struct work_struct      defer_work;      /* interrupt bottom half worker */
#endif
	onebox_netbuf_head_t    deferred_rx_queue; 	
#ifdef USE_TASKLETS
	struct tasklet_struct	int_bh_tasklet;
#endif /* USE_TASKLETS */
	uint8   coex_mode;
 	uint8 usb_intf_suspend; //Indicates the usb intf is suspended
	uint8 usb_in_deep_ps;
	struct timer_list usb_resume_timer;
	uint8 *segment;
	uint32 fw;
	uint32 write_fail;
	struct proc_dir_entry *onebox_entry;
	uint8 proc_name[23];
	uint8 host_intf_type;
	struct driver_assets *d_assets;
	uint32 rx_packtes_dropped;
#ifdef SDIO_DEBUG
	struct timer_list sdio_debug_timer;
	uint32 timer_sdio_int_counter;
	uint32 timer_total_sdio_msdu_pending_intr;
	uint32 timer_total_sdio_unknown_intr;
	uint32 timer_buf_status_interrupts;
	uint32 timer_sdio_intr_status_zero;
	uint32 total_cmd52_skipped;
#endif
};

/* coex */

void deferred_rx_packet_parser(struct work_struct *work);
void deferred_rx_tasklet(unsigned long d);
ONEBOX_STATUS onebox_iap_config(PONEBOX_ADAPTER, uint8, struct mfi_challenge *);

/***************** END DRIVER DATA STRUCTURE TEMPLATES ******************/

void dump_debug_frame(uint8 *msg, int32 msg_len);
struct onebox_coex_osi_operations *onebox_get_coex_osi_operations(void);
struct proc_dir_entry *init_proc_fs(void *adapter, uint8 *proc_name);
int32 panic_stop_all_activites(PONEBOX_ADAPTER adapter, uint32 reg_to_ack);
uint8 deploy_packet_to_assets(PONEBOX_ADAPTER adapter, 
			      netbuf_ctrl_block_t *nb_deploy);

void update_tx_status(struct driver_assets *, uint8 proto_id);
#ifdef GPIO_HANDSHAKE
void gpio_deinit(void);
void gpio_init(void);
void set_host_status (int value);
int get_device_status (void);
void gpio_host_tx_intention(PONEBOX_ADAPTER adapter,bool gpio_value);
#endif
void sleep_entry_recvd(PONEBOX_ADAPTER adapter);
void sleep_exit_recvd(PONEBOX_ADAPTER adapter);
ONEBOX_STATUS wlan_module_init(struct driver_assets *d_assets);
ONEBOX_STATUS bt_module_init(struct driver_assets *d_assets);
ONEBOX_STATUS zb_module_init(struct driver_assets *d_assets);
#endif

