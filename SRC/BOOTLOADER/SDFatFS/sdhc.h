/*----------------------------------------------------------------------
 *
 * Filename: sdhc.h
 *
 * Contents: 
 *
 * Authors: YMK,
 * 
 * History:
 * 	- 08.OCT.2005        by Youngmin.Kim
 * 		1) 1st Made
 * 
 *
 * Notes:
 *
 * Copyright (c) 2005 SAMSUNG Electronics.
 *
 *----------------------------------------------------------------------
 */
#ifndef _SDHC_H_
#define _SDHC_H_

#include <bsp.h>
//#include "fat.h"
#include <s3c6410.h>


typedef unsigned long   ULONG;
typedef unsigned long	u32;
typedef unsigned short	u16;
typedef unsigned char	u8;

#ifdef __cplusplus
extern "C" {
#endif

// SDHC SFR Control Register index.
//////////////////////////////////////////////////////////////////////////////////////////////////

// SDHC Transfer Mode.
typedef enum {
	SDHC_POLLING_MODE = 0,
	SDHC_INTERRUPT_MODE,
	SDHC_SDMA_MODE,
	SDHC_ADMA2_MODE
} SDHC_operation;

// SDHC Channel Number.
typedef enum {
	SDHC_CHANNEL_0 = 0,
	SDHC_CHANNEL_1,
	SDHC_CHANNEL_2,
	SDHC_CHANNEL_CNT
} SDHC_channel;

// SDHC Card Type.
typedef enum {
	SDHC_SD_CARD=0,
	SDHC_MMC_CARD,
	SDHC_CE_ATA_CARD,
	SDHC_SDIO_CARD,
} SDHC_card_type;

// Clock Source
typedef enum {
	SDHC_HCLK=1,
	SDHC_EPLL=2,
	SDHC_EXTCLK=3
} SDHC_clockSource;

// SD/MMC Speed Mode
typedef enum _SDHC_SpeedMode {
	SDHC_NORMAL_SPEED = 0,
	SDHC_HIGH_SPEED = 1
} SDHC_SpeedMode;

// Command Type
typedef enum _SDHC_CommandType {
	SDHC_CMD_BC_TYPE,		// broadcast commands (bc), no response
	SDHC_CMD_BCR_TYPE,		// broadcast commands with response (bcr)
	SDHC_CMD_AC_TYPE,		// addressed (point-to-point) commands (ac), no data transfer on DAT lines
	SDHC_CMD_ADTC_TYPE,		// addressed (point-to-point) data transfer commands (adtc), data transfer on DAT lines
} SDHC_CommandType;

// Command Response Type.
typedef enum _SDHC_ResponseType {
	SDHC_RES_NO_TYPE=0,
	SDHC_RES_R1_TYPE,
	SDHC_RES_R1B_TYPE,
	SDHC_RES_R2_TYPE,
	SDHC_RES_R3_TYPE,
	SDHC_RES_R4_TYPE,
	SDHC_RES_R5_TYPE,
	SDHC_RES_R6_TYPE,
	SDHC_RES_R7_TYPE,
} SDHC_ResponseType;

typedef enum {
	SDHC_BYTE_MODE=0,
	SDHC_BLOCK_MODE
} SDHC_transfer_mode;

// Common SD/MMC Structure.
typedef struct {
	u8* m_uBaseAddr;
	SDHC_operation  m_eOpMode;
	SDHC_card_type m_eCardType;
	SDHC_transfer_mode m_eTransMode;
	SDHC_channel m_eChannel;
	SDHC_clockSource m_eClockSource;
	u32  m_uClockDivision;
	u16  m_uRca;
	u16	 m_usTransMode;
	u16  m_usClkCtrlReg;
	u16 m_uRemainBlock;
	u8	m_uCCSResponse;		// CCS signal for CE-ATA
	u8   m_ucSpecVer;
	u8   m_ucHostCtrlReg;
	u8   m_ucBandwidth;
	u32 * m_uBufferPtr;
	// -- Card Information
	u32 m_uStartBlockPos;		// startBlock Position. - for Test Case usage.
	u16 m_uOneBlockSize;		// multi block count.	- fot Test Case usage.
	u16 m_sReadBlockLen;		// One Block Size = (1<<READ_BL_LEN)
	u16 m_sReadBlockPartial;
	u16 m_sCSize;
	u16 m_sCSizeMult;			// CardSize = (1<<m_sReadBlockLen)*(m_sCSize+1)*(1<<(m_sCSizeMult+2))/1048576
	u16 m_sNormalStatus;		// normal status
	u16 m_sErrorStatus;			// error status.
	//
	u8 m_ucIntChannelNum;		// VIC Interrupt Number.
	void (*m_fIntFn)(void);	// DMA Interrupt Handler Pointer.
} SDHC;

void SDHC_InitCh(SDHC_channel eCh, SDHC *sCh);
u8 SDHC_OpenMedia(SDHC_clockSource eClkSrc, SDHC* sCh);
u8 SDHC_OpenMediaWithMode(u32 uBusWidth, SDHC_operation eOpMode, SDHC_clockSource eClkSrc,
	u32 uSdClkDivisor, SDHC_channel channel, SDHC* sCh);
void SDHC_CloseMedia(SDHC* sCh);

void SDHC_WriteOneBlock( SDHC* sCh );
void SDHC_ReadOneBlock( SDHC* sCh );

u8 SDHC_WriteBlocks(u32 uStBlock, u16 uBlocks, u32 uBufAddr, SDHC* sCh);
u8 SDHC_ReadBlocks(u32 uStBlock, u16 uBlocks, u32 uBufAddr, SDHC* sCh);
u8 SDHC_EraseBlocks( SDHC* sCh, u32 startBlock, u32 endBlock );
u8 SDHC_IsErrorOccured(SDHC* sCh);
	
u8 SDHC_IdentifyCard(SDHC* sCh);
void SDHC_ResetController(SDHC* sCh);
void SDHC_SetSdClock(u32 uDivisor, SDHC* sCh, SDHC_SpeedMode speed, u32 workingFreq );
u8 SDHC_IssueCommand( SDHC* sCh, u16 uCmd, u32 uArg, SDHC_CommandType cType, SDHC_ResponseType rType );
u8 SDHC_GetSdScr(SDHC* sCh);
u8 SDHC_ReadMMCExtCSD(SDHC* sCh);
void SDHC_GetResponseData(u32 uCmd, SDHC* sCh);

u8 SDHC_SetSDOCR(SDHC* sCh);
u8 SDHC_SetMmcOcr(SDHC* sCh);

u8 SDHC_WaitForCard2TransferState(SDHC* sCh);

void SDHC_ClearErrInterruptStatus(SDHC* sCh);

void SDHC_SetTransferModeReg(u32 MultiBlk, u32 DataDirection, u32 AutoCmd12En, u32 BlockCntEn, u32 DmaEn, SDHC* sCh);
void SDHC_SetArgumentReg(u32 uArg, SDHC* sCh);
void SDHC_SetSdhcInterruptEnable(u16 uNormalIntStatusEn, u16 uErrorIntStatusEn, u16 uNormalIntSigEn, u16 uErrorIntSigEn, SDHC* sCh);
void SDHC_SetHostCtrlSpeedMode(SDHC_SpeedMode eSpeedMode, SDHC* sCh);
void SDHC_SetSdhcCardIntEnable(u8 ucTemp, SDHC* sCh);
void SDHC_SetSdClockOnOff(u8 uOnOff, SDHC* sCh);
	
u8 SDHC_SetDataTransferWidth(SDHC* sCh);
u8 SDHC_SetSdCardSpeedMode(SDHC_SpeedMode eSpeedMode, SDHC* sCh);
u8 SDHC_SetMmcSpeedMode(SDHC_SpeedMode eSpeedMode, SDHC* sCh);
void SDHC_DisplayCardInformation(SDHC * sCh);
u8 SDHC_SetGPIO(SDHC_channel channelNum, int channel_width);
void SDHC_CardDetection(SDHC* sCh);
void SDHC_PowerSelection(SDHC* sCh);
void SDHC_Set_InitClock( SDHC* sCh );
u8 SDHC_OpenMediaMulti(SDHC_clockSource eClkSrc, SDHC* sCh);


//Normal Interrupt Signal Enable
#define	SDHC_ERROR_INTERRUPT_EN				(1<<15)
#define	SDHC_SD_ADDRESS_INT3_EN				(1<<14)
#define	SDHC_SD_ADDRESS_INT2_EN				(1<<13)
#define	SDHC_SD_ADDRESS_INT1_EN				(1<<12)
#define	SDHC_SD_ADDRESS_INT0_EN				(1<<11)
#define	SDHC_READWAIT_SIG_INT_EN			(1<<10)
#define	SDHC_CCS_INTERRUPT_STATUS_EN		(1<<9)
#define	SDHC_CARD_SIG_INT_EN					(1<<8)
#define	SDHC_CARD_REMOVAL_SIG_INT_EN		(1<<7)
#define	SDHC_CARD_INSERT_SIG_INT_EN			(1<<6)
#define	SDHC_BUFFER_READREADY_SIG_INT_EN	(1<<5)
#define	SDHC_BUFFER_WRITEREADY_SIG_INT_EN	(1<<4)
#define	SDHC_DMA_SIG_INT_EN					(1<<3)
#define	SDHC_BLOCKGAP_EVENT_SIG_INT_EN		(1<<2)
#define	SDHC_TRANSFERCOMPLETE_SIG_INT_EN	(1<<1)
#define	SDHC_COMMANDCOMPLETE_SIG_INT_EN	(1<<0)

// Error Interrupt Signal Enable.
#define	SDHC_ADMA2_ERROR						(1<<9)
#define	SDHC_AUTO_CMD12_ERROR				(1<<8)
#define	SDHC_CURRENT_LIMIT_ERROR				(1<<7)
#define	SDHC_DATA_END_BIT_ERROR				(1<<6)
#define	SDHC_DATA_CRC_ERROR					(1<<5)
#define	SDHC_DATA_TIME_OUT_ERROR			(1<<4)
#define	SDHC_CMD_INDEX_ERROR					(1<<3)
#define	SDHC_CMD_END_BIT_ERROR				(1<<2)
#define	SDHC_CMD_CRC_ERROR					(1<<1)
#define	SDHC_CMD_TIMEOUT_ERROR				(1<<0)

// ADMA2 Error status register.
#define	SDHC_ADMA_FINAL_BLOCK_TRANSFERRED	(1<<10)
#define	SDHC_ADMA_CONTINUE_REQUEST			(1<<9)
#define	SDHC_ADMA_INTERRUPT_STATUS			(1<<8)
#define	SDHC_ADMA_LENGTH_MISMATCH_ERR		(1<<2)
#define	SDHC_ADMA_ERROR_STATUS				(1<<0)

  
#ifdef __cplusplus
}
#endif

#endif

