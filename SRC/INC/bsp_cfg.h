//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
//------------------------------------------------------------------------------
//
//  File:  bsp_cfg.h
//
//  This file contains system constant specific for SMDK6410 board.
//
#ifndef __BSP_CFG_H
#define __BSP_CFG_H

#include <soc_cfg.h>

//------------------------------------------------------------------------------
//
//  Define:  BSP_DEVICE_PREFIX
//
//  Prefix used to generate device name for bootload/KITL
//
#define BSP_DEVICE_PREFIX       "TBT6410"        // Device name prefix

#define PWRCTL_HITEG_FACTORY_DEFAULTS ((1<<0) | (1<<1) | (1<<4))


// We define the maximum values for the S3C6410 as reported by SAMSUNG
// However we suggest using in the max resolution 8Bit depth for the framebuffer to get 
// fluid visuals....no OpenGL on that one though....but check out the TFT controller and 2D Accel. !!!!

#define LCD_MAX_WIDTH	1024
#define LCD_MAX_HEIGHT	 768
#define LCD_MAX_BPP		  32

#define NO_DISPLAY		(~0)

//------------------------------------------------------------------------------
// SMDK6410 Audio Sampling Rate
//------------------------------------------------------------------------------
#define AUDIO_44_1KHz        (44100)
#define AUDIO_48KHz            (48000)
#define AUDIO_SAMPLE_RATE    (AUDIO_44_1KHz)        // Keep sync with EPLL Fout

//------------------------------------------------------------------------------
// SMDK6410 UART Debug Port Baudrate
//------------------------------------------------------------------------------
#define DEBUG_UART0         (0)
#define DEBUG_UART1         (1)
#define DEBUG_UART2         (2)
#define DEBUG_UART3         (3)
#define DEBUG_BAUDRATE      (115200)

//------------------------------------------------------------------------------
// SMDK6410 NAND Flash Timing Parameter
// Need to optimize for each HClock value.
// Default vaule is 7.7.7. this value has maximum margin. 
// so stable but performance cannot be max.
//------------------------------------------------------------------------------
#define NAND_TACLS         (7)
#define NAND_TWRPH0        (7)
#define NAND_TWRPH1        (7)

//------------------------------------------------------------------------------
// SMDK6410 CF Interface Mode
//------------------------------------------------------------------------------
#define CF_INDIRECT_MODE    (0)        // MemPort0 Shared by EBI
#define CF_DIRECT_MODE      (1)        // Independent CF Interface
#define CF_INTERFACE_MODE   (CF_DIRECT_MODE)

// Added for DM9000
#ifndef SYSINTR_DM9000A1
#define SYSINTR_DM9000A1     (SYSINTR_FIRMWARE+2)	// for DM9000 Adp1, nCS4, 18
#endif
//------------------------------------------------------------------------------
// TenByTen6410 Keypad Layout
//------------------------------------------------------------------------------
#define LAYOUT0                (0)        // 8*8 Keypad board
#define LAYOUT1                (1)        // On-Board Key
#define LAYOUT2                (2)        // Qwerty Key board
#define MATRIX_LAYOUT        (LAYOUT1)

#endif // __BSP_CFG_H

