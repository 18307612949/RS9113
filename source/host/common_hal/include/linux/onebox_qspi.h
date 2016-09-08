/**
 * @file onebox_eeprom.h
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
 * This file contians the datastructures / macros and function prototypes 
 * related to eeprom module
 * 
 */

#ifndef __ONEBOX_QSPI_H
#define __ONEBOX_QSPI_H


#include "onebox_eeprom.h"
/*****************************************
 *        Qspi register defines
 *****************************************/
//! cmd len will be 8 bits
#define CMD_LEN                      8
//! reg bit
#define RD_FIFO_EMPTY                BIT(7)
//! reg bit
#define BUSY                         BIT(0)
//! reg bit
#define FULL_DUPLEX_EN               BIT(22)  
//! reg bit
#define HW_CTRL_MODE                 BIT(25)
//! reg bit
#define AUTO_MODE                    BIT(6) 
//! reg bit
#define EXTRA_BYTE_EN                BIT(18)
//! reg bit
#define AUTO_RD_SWAP                 BIT(0)
//! reg bit
#define HW_CTRLD_QSPI_MODE_CTRL_SCLK BIT(14)
//! reg bit
#define AUTO_MODE_FSM_IDLE_SCLK      BIT(10)
//! reg bit
#define CSN_ACTIVE                   BIT(0)  
//! reg bit
#define QSPI_PREFETCH_EN             BIT(4)
//! reg bit
#define QSPI_WRAP_EN                 BIT(5)
//! reg bit
#define READ_TRIGGER                 BIT(2)
//! reg bit
#define WRITE_TRIGGER                BIT(1)

/******************************************
 *              FLASH CMDS 
 ******************************************/
//! Write enable cmd
#define WREN                         0x06
//! Write disable cmd
#define WRDI                         0x04
//! Read status reg cmd
#define RDSR                         0x05
//! chip erase cmd
#define CHIP_ERASE                   0xC7
//! block erase cmd
#define BLOCK_ERASE                  0xD8 
//! sector erase cmd
#define SECTOR_ERASE                 0x20      
//! high speed rd cmd
#define HISPEED_READ                 0x0B 
//! rd cmd
#define READ_ONLY                    0x03

/****************************************
 *        SST25 specific cmds 
 ****************************************/
//! Write status reg cmd
#define WRSR                         0x01
//! Enable Write status reg cmd
#define EWSR                         0x50                              
//! Auto address incremental rd cmd
#define AAI                          0xAF
//! Byte program cmd
#define BYTE_PROGRAM                 0x02

/****************************************
 *       SST26 specific cmds 
 ***************************************/
//! Enable quad IO
#define EQIO                         0x38 
//! Reset quad IO
#define RSTQIO                       0xFF
//! wrap : set burst
#define SET_BURST                    0xC0
//! wrap : read cmd
#define READ_BURST                   0x0C
//! Jump : page index read
#define READ_PI                      0x08
//! Jump : Index read
#define READ_I                       0x09    
//! Jump : Block Index read
#define READ_BI                      0x10
//! Page program cmd 
#define PAGE_PROGRAM                 0x02  
//! write suspend cmd
#define Write_Suspend                0xB0        
//! write resume cmd
#define Write_Resume                 0x30  
//! read block protection reg
#define RBPR                         0x72
//! Write block protection reg
#define WBPR                         0x42
//! Lockdown block protection reg 
#define LBPR                         0x8D

/****************************************
 * WINBOND + AT + MACRONIX specific cmds
 ***************************************/
//! fast read dual output
#define FREAD_DUAL_O                 0x3B
//! fast read quad output
#define FREAD_QUAD_O                 0x6B


/****************************************
 * WINBOND + MACRONIX specific cmds 
 ***************************************/ 
//! fast read dual IO
#define FREAD_DUAL_IO                0xBB
//! fast read quad IO
#define FREAD_QUAD_IO                0xEB


/****************************************
 *       WINBOND specific cmds
 ***************************************/
//! Octal word read (A7-A0 must be 00)
#define OCTAL_WREAD                  0xE3
//! Enable high performance cmd
#define HI_PERFMNC                   0xA3


/****************************************
 *        ATMEL specific cmds
 ***************************************/
//! write config reg
#define WCON                         0x3E
//! read config reg
#define RCON                         0x3F
//! supported upto 100MHz
#define HI_FREQ_SPI_READ             0x1B


/****************************************
 *    MACRONIX specific write cmds 
 ***************************************/
//! Address and data in quad
#define QUAD_PAGE_PROGRAM            0x38



/****************************************
 * ATMEL + WINBOND specific write cmds
 ***************************************/
//! Only data in quad mode
#define QUAD_IN_PAGE_PROGRAM         0x32



/****************************************
 * ATMEL specific write cmds
 ***************************************/
//! Data in dual 
#define DUAL_IN_PAGE_PROGRAM         0xA2


/*************************************** 
 *        Defines for arguments 
 **************************************/

//! disable hw ctrl
#define DIS_HW_CTRL                  1
//! donot disable hw ctrl
#define DNT_DIS_HW_CTRL              0

//! 32bit hsize
#define _32BIT                       3
//! 24bit hsize is not supported, so reserved
//      reserved                     2
//! 16bit hsize
#define _16BIT                       1
//! 8bit hsize
#define _8BIT                        0


//! default spi_configs are defined here
static spi_config_t spi_default_configs = {
  {
    .inst_mode         = SINGLE_MODE,
    .addr_mode         = SINGLE_MODE,
    .data_mode         = SINGLE_MODE,
    .dummy_mode        = SINGLE_MODE,    
    .extra_byte_mode   = SINGLE_MODE,
    .prefetch_en       = EN_PREFETCH,
    .dummy_W_or_R      = DUMMY_READS,
    .extra_byte_en     = 0,
    .d3d2_data         = 3,
    .continuous        = DIS_CONTINUOUS,
    .read_cmd          = READ_ONLY,
    .flash_type        = MICRON_QUAD_FLASH,
    .no_of_dummy_bytes = 0
  },
  {
    //.auto_mode         = EN_MANUAL_MODE,
    .auto_mode         = EN_AUTO_MODE,
    .cs_no             = CHIP_ZERO,
    .jump_en           = DIS_JUMP,
    .neg_edge_sampling = NEG_EDGE_SAMPLING, 
    .qspi_clk_en       = QSPI_FULL_TIME_CLK,
    .protection        = DNT_REM_WR_PROT,
    .dma_mode          = NO_DMA,
    .swap_en           = SWAP,
    .full_duplex       = IGNORE_FULL_DUPLEX,
    .wrap_len_in_bytes = NO_WRAP,
    .mode_0_or_3       = MODE_0,
    .addr_width        = _24BIT_ADDR,
    .jump_inst         = READ_PI,
    .dummys_4_jump     = 1,
    .num_prot_bytes    = _10BYTES_LONG
  }
};

#endif
