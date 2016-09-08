/**
 * @file onebox_bootup_params.h
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
 * This file contians the bootup parameters to be loaded to firmware.
 */

#ifndef __OENBOX_BOOTPARAMS_HEADER_H__
#define __OENBOX_BOOTPARAMS_HEADER_H__

#include "wlan_common.h" 

#define TA_PLL_DIS            BIT(5)
#define TA_PLL_M_VAL          46         
#define TA_PLL_N_VAL          3        
#define TA_PLL_P_VAL          3        //! 80mhz 8 = 40
                                      
#define PLL960_DIS            (0x9 << 4)          
#define PLL960_M_VAL          0x14    
#define PLL960_N_VAL          0    
#define PLL960_P_VAL          5

#define BBP_CLK_DIV_FAC       1 
#define LMAC_CLK_DIV_FAC      1  
#define AFE_CLK_DIV_FAC       1 
#define BB_LMAC_160_80        BIT(12)
#define CLK_2_SPARE_GATES     BIT(14) | BIT(13)


BOOTUP_PARAMETERS boot_params =
{
  .magic_number = 0x5aa5,
  .crystal_good_time = 0x0,
  .valid = 0x1e3,
  .reserved_for_valids = 0x0,
  .bootup_mode_info = 0x0,
  .digital_loop_back_params = 0x0,
  .rtls_timestamp_en = 0x0,
  .host_spi_intr_cfg = 0x0,
  .device_clk_info = {
    //! Wifi params
    {
      .pll_config_g = {
        .tapll_info_g = {
          .pll_config_register_1 = (TA_PLL_N_VAL << 8) | TA_PLL_M_VAL,
          .pll_config_register_2 = (TA_PLL_P_VAL) // | TA_PLL_DIS,  
        },
        .pll960_info_g = {
          .pll_config_register_1 = (PLL960_P_VAL << 8) | (PLL960_N_VAL),
          .pll_config_register_2 = PLL960_M_VAL,
          .pll_config_register_3 = 0x0 // | PLL960_DIS,
        },
        .afepll_info_g = {
          .pll_config_register = 0x9f0,
        }
      },
      .switch_clk_g = {
        .switch_umac_clk = 0x1,
        .switch_qspi_clk = 0x0,
        .switch_slp_clk_2_32 = 0x0,
        .switch_bbp_lmac_clk_reg = 0x1,
        .switch_mem_ctrl_cfg = 0x0,
        .reserved = 0x0,
        .bbp_lmac_clk_reg_val = 0x1121,
        .umac_clock_reg_config = 0x48,
        .qspi_uart_clock_reg_config = 0x0
      }
    },
    //! Bluetooth params
    {
      .pll_config_g = {
        .tapll_info_g = {
          .pll_config_register_1 = (TA_PLL_N_VAL << 8) | TA_PLL_M_VAL,
          .pll_config_register_2 = (TA_PLL_P_VAL) // | TA_PLL_DIS,  
        },
        .pll960_info_g = {
          .pll_config_register_1 = (PLL960_P_VAL << 8) | (PLL960_N_VAL),
          .pll_config_register_2 = PLL960_M_VAL,
          .pll_config_register_3 = 0x0 // | PLL960_DIS,
        },
        .afepll_info_g = {
          .pll_config_register = 0x9f0,
        }
      },
      .switch_clk_g = {
        .switch_umac_clk = 0x0,
        .switch_qspi_clk = 0x0,
        .switch_slp_clk_2_32 = 0x0,
        .switch_bbp_lmac_clk_reg = 0x0,
        .switch_mem_ctrl_cfg = 0x0,
        .reserved = 0x0,
        .bbp_lmac_clk_reg_val = 0x0,
        .umac_clock_reg_config = 0x0,
        .qspi_uart_clock_reg_config = 0x0
      }
    },
    //! Zigbee params
    {
      .pll_config_g = {
        .tapll_info_g = {
          .pll_config_register_1 = (TA_PLL_N_VAL << 8) | TA_PLL_M_VAL,
          .pll_config_register_2 = (TA_PLL_P_VAL) // | TA_PLL_DIS,  
        },
        .pll960_info_g = {
          .pll_config_register_1 = (PLL960_P_VAL << 8) | (PLL960_N_VAL),
          .pll_config_register_2 = PLL960_M_VAL,
          .pll_config_register_3 = 0x0 // | PLL960_DIS,
        },
        .afepll_info_g = {
          .pll_config_register = 0x9f0,
        }
      },
      .switch_clk_g = {
        .switch_umac_clk = 0x0,
        .switch_qspi_clk = 0x0,
        .switch_slp_clk_2_32 = 0x0,
        .switch_bbp_lmac_clk_reg = 0x0,
        .switch_mem_ctrl_cfg = 0x0,
        .reserved = 0x0,
        .bbp_lmac_clk_reg_val = 0x0,
        .umac_clock_reg_config = 0x0,
        .qspi_uart_clock_reg_config = 0x0
      }
    }
  },
  //! ULP Params
  .buckboost_wakeup_cnt = 0x0,
  .pmu_wakeup_wait = 0x0,
  .shutdown_wait_time = 0x0,
  .pmu_slp_clkout_sel = 0x0,
  .wdt_prog_value = 0x0,
  .wdt_soc_rst_delay = 0x0,
  .dcdc_operation_mode = 0x0, 
  .soc_reset_wait_cnt = 0x0
};

  

#endif
