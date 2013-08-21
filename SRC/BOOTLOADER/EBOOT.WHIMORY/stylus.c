/**************************************************************************************
* 
*	Project Name : S3C6410 Validation
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is only for validating functions of the S3C6410.
*		Anybody can use this software without our permission.
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : adcts.c
*  
*	File Description : This file implements the driver functions for ADC/TS.
*
*	Author : Sunil,Roe
*	Dept. : AP Development Team
*	Created Date : 2007/1/2
*	Version : 0.1 
* 
*	History
*	- Created(Sunil,Roe 2007/1/2)
*   
**************************************************************************************/

#include <windows.h>
#include <halether.h>
#include <bsp.h>
#include <specstrings.h>
#include <s3c6410_adc.h>
#include <s3c6410_base_regs.h>

#define	ENABLE					1
#define	DISABLE					0
#define	SELECT					1
#define	DESELECT				0

#define	STYLUS_DOWN			0
#define	STYLUS_UP				1

#define	ADCTS_TOUCH_INIT		2
#define	ADCTS_TOUCH_DOWN	0
#define	ADCTS_TOUCH_UP		1

typedef enum ADCTS_Id
{
	eADCTS_ADCCON = 0,
	eADCTS_ADCTSC = 1,
	eADCTS_ADCDLY = 2,
	eADCTS_ADCDAT0 = 3,
	eADCTS_ADCDAT1 = 4,
	eADCTS_ADCUPDN = 5,
	eADCTS_ADCCLRINT = 6,
	eADCTS_ADCCLRWK = 8,
}
eADCTS_Id;

typedef enum ADCTS_MeasureModeId
{
	eADCTS_MEASURE_NOP = 0,
	eADCTS_MEASURE_MANUAL_X = 1,
	eADCTS_MEASURE_MANUAL_Y = 2,
	eADCTS_MEASURE_AUTO_SEQ = 3,
	eADCTS_MEASURE_FOR_STYLUS = 4,
}
eADCTS_MeasureModeId;

typedef struct ADCTS_Inform
{	
	ULONG	uADCSampleData;	
	unsigned char	ucADCflag;
	unsigned char	ucTouchStatus;
}
ADCTS_oInform;

#define ADCTS			( (volatile S3C6410_ADC_REG *) (g_pADCTSBase))
#define	ADCTS_PRSCVL		10


// define global variables
static volatile S3C6410_ADC_REG *				g_pADCTSBase;
volatile	ADCTS_oInform	g_oADCTSInform;
BOOL ADCTS_Init(void);
void ADCTS_Test(void);

void ADC_ConvertADCNormallyInPolling(void);
void ADC_ConvertADCNormallyInADCInterrupt(void);
void ADCTS_ConvertXYPosSeparatelyInPolling(void);
void ADCTS_ConvertXYPosSequentiallyInPolling(void);
void ADCTS_StylusCheckInPENDNInterrupt(void);
void ADCTS_StylusTrackingUsingAutoConversionInPENDNInterruptADCPolling(void);
void ADC_CheckADCLinearity(void);

/*---------------------------------- APIs of rADCCON ---------------------------------*/
void ADC_InitADCCON(void);
BOOL ADC_IsEOCFlag(void);
void ADC_EnablePrescaler(unsigned char);
void ADC_SetPrescaler(unsigned char);
void ADC_SelectADCCh(unsigned char);
void ADC_SelectStandbyMode(unsigned char);
void ADC_EnableReadStart(unsigned char);
void ADC_EnableStart(unsigned char);
BOOL ADC_IsAfterStartup(void);
/*---------------------------------- APIs of rADCTSC ---------------------------------*/
void ADCTS_SetMeasureMode(unsigned char );
void ADCTS_SetStylusDetectMode(unsigned char );
/*---------------------------------- APIs of rADCDLY ---------------------------------*/
void ADCTS_SetDelayClkSrc(unsigned char );
void ADCTS_SetDelay(ULONG );
/*---------------------------------- APIs of rADCDATx ---------------------------------*/
unsigned char ADCTS_GetXPStylusIntState(void);
unsigned char ADCTS_GetYPStylusIntState(void);
ULONG ADCTS_GetXPData(void);
ULONG ADCTS_GetYPData(void);
/*---------------------------------- APIs of INT Clear Registers ---------------------------------*/
void ADCTS_SetADCClearInt(void);
void ADCTS_SetADCClearWKInt(void);

/*---------------------------------- APIs of general ADCTS ---------------------------------*/
ULONG ADCTS_GetRegValue(unsigned char);
void ADCTS_SetRegValue(unsigned char, ULONG);

/*---------------------------------- ISR Routines Definition ---------------------------------*/

void ADC_NormalISR(void);
void ADCTS_StylusCheckISR(void);
void ADCTS_StylusTrackingISR(void);

static ULONG g_PCLK;

//////////
// Function Name : ADCTS_Init
// Function Description : This function initializes an ADCTS.
// Input : 	NONE 
// Output : 	TRUE - Memory Device is reset
//			FALSE - Memory Device is not reset because of ERROR
// Version : v0.1
BOOL ADCTS_Init(void)
{
	ULONG uBaseAddress=0;

	g_pADCTSBase = (S3C6410_ADC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_ADC, FALSE);
	g_PCLK=System_GetPCLK();

	ADCTS_SetADCClearInt();
	ADCTS_SetADCClearWKInt();
	
	return TRUE;
}

//////////
// Function Name : ADC_ConvertADCNormallyInPolling
// Function Description : This function tests ADC Normal Conversion in Polling mode.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADC_ConvertADCNormallyInPolling(void)
{
	unsigned char	ucADCChannelCnt = 0;

	EdbgOutputDebugString("The ADC_IN are adjusted to the following values.\n");        
	EdbgOutputDebugString("Push any key to exit!!!\n");    
	EdbgOutputDebugString("ADC conv. freq.=%d(Hz)\n",(ULONG)(g_PCLK/(ADCTS_PRSCVL+1.)));

	ADC_InitADCCON();

	while(getChar()==0) 
	{
		for(ucADCChannelCnt=0 ; ucADCChannelCnt<8; ucADCChannelCnt++) 
		{
			ADC_SelectADCCh(ucADCChannelCnt);	//	Channel setup
			ADC_EnableStart(ENABLE);

			while (! ADC_IsAfterStartup() ) ;	//	Wait for begin sampling

			while (! (ADC_IsEOCFlag()));			//	Wait for the EOC

			EdbgOutputDebugString("Ain%d: %04d ",ucADCChannelCnt, ADCTS_GetXPData());
		}
		EdbgOutputDebugString("\n");
	}
}

//////////
// Function Name : ADC_ConvertADCNormallyInADCInterrupt
// Function Description : This function tests ADC Normal Conversion in Interrupt mode.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
extern void InitializeInterrupt(void);
void ADC_ConvertADCNormallyInADCInterrupt(void)
{

	unsigned char	ucADCChannelCnt = 0;
	    
	EdbgOutputDebugString("The ADC_IN are adjusted to the following values.\n");        
	EdbgOutputDebugString("Push any key to exit!!!\n");    
	EdbgOutputDebugString("ADC conv. freq.=%d(Hz)\n",(ULONG)(g_PCLK/(ADCTS_PRSCVL+1.)));

	ADC_InitADCCON();
	InitializeInterrupt();
	VIC_InterruptEnable(PHYIRQ_ADC);
	
	while(GetKey()==0)
	{
    
		for(ucADCChannelCnt = 0 ;ucADCChannelCnt<8; ucADCChannelCnt++) 
		{
			ADC_SelectADCCh(ucADCChannelCnt);	//	Channel setup

			g_oADCTSInform.ucADCflag = 0;
			ADC_EnableStart(ENABLE);
			
			while(g_oADCTSInform.ucADCflag == 0);

			EdbgOutputDebugString("Ain%d: %04d ",ucADCChannelCnt,g_oADCTSInform.uADCSampleData);
		}
//		Getc();
		EdbgOutputDebugString("\n");
	}

	VIC_InterruptDisable(PHYIRQ_ADC);
}

//////////
// Function Name : ADCTS_ConvertXYPosSeparatelyInPolling
// Function Description : This function tests ADCTS Manual Conversion of X/Y Position in Polling mode.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADCTS_ConvertXYPosSeparatelyInPolling(void)
{
	ULONG uXDat = 0;
	ULONG uYDat = 0;
	EdbgOutputDebugString("[ Separate X/Y Position measurement mode ]\n");
   
   	while(GetKey()==0) 
	{
		//------- Get X-Position Data -------------
		ADCTS_SetMeasureMode(eADCTS_MEASURE_MANUAL_X);

		ADC_InitADCCON();
		ADC_SelectADCCh(5);		// Channel Setup

		ADC_EnableStart(ENABLE);

		while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
		while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

		uXDat	= ADCTS_GetXPData();

		//------- Get Y-Position Data -------------
		ADCTS_SetMeasureMode(eADCTS_MEASURE_MANUAL_Y);

		ADC_InitADCCON();
		ADC_SelectADCCh(7);		// Channel Setup

		ADC_EnableStart(ENABLE);

		while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
		while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

		uYDat	= ADCTS_GetYPData();

		EdbgOutputDebugString("X : %d, Y : %d\n",uXDat,uYDat);
	}

}

//////////
// Function Name : ADCTS_ConvertXYPosSequentiallyInPolling
// Function Description : This function tests ADCTS Auto Sequential Conversion of X/Y Position in Polling mode.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADCTS_ConvertXYPosSequentiallyInPolling(void)
{
	ULONG uXDat = 0;
	ULONG uYDat = 0;
	EdbgOutputDebugString("[ Auto Sequential X/Y Position measurement mode ]\n");
   
   	while(GetKey()==0) 
	{
		ADCTS_SetMeasureMode(eADCTS_MEASURE_AUTO_SEQ);

		ADC_InitADCCON();

		ADC_EnableStart(ENABLE);

		while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
		while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

		uXDat	= ADCTS_GetXPData();
		uYDat	= ADCTS_GetYPData();

		EdbgOutputDebugString("X : %d, Y : %d\n",uXDat,uYDat);
   	}
}

//////////
// Function Name : ADCTS_StylusCheckInPENDNInterrupt
// Function Description : This function tests ADCTS Stylus Check Using PenDown Interrupt.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADCTS_StylusCheckInPENDNInterrupt(void)
{
	ADC_InitADCCON();
	ADCTS_SetDelay(50000);		//	Normal conversion mode delay about (1/50M)*50000=1ms

	EdbgOutputDebugString("[ADC touch screen test.]\n");

	ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);

	Delay(10000);

	INTC_SetVectAddr(NUM_PENDN, ADCTS_StylusCheckISR);
	INTC_Enable(NUM_PENDN);

	EdbgOutputDebugString("Type any key to exit!!!\n");
	EdbgOutputDebugString("Stylus Down, please...... \n");
	Getc();

	INTC_Disable(NUM_PENDN);
	
}

//////////
// Function Name : ADCTS_StylusTrackingUsingAutoConversionInPENDNInterrupt
// Function Description : This function tests ADCTS Auto Sequential Conversion of X/Y Position in Polling mode Using PenDown Interrupt.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADCTS_StylusTrackingUsingAutoConversionInPENDNInterruptADCPolling(void)
{
	ULONG uXDat = 0;
	ULONG uYDat = 0;

	EdbgOutputDebugString("\n\n[ADCTS touch screen Tracking test.]\n");

	g_oADCTSInform.ucTouchStatus	=	ADCTS_TOUCH_INIT;
	ADC_InitADCCON();
	ADCTS_SetDelay(50000);

	ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);

	Delay(10000);

	INTC_SetVectAddr(NUM_PENDN, ADCTS_StylusTrackingISR);
	INTC_Enable(NUM_PENDN);

    	EdbgOutputDebugString("\nPress any key to exit!!!\n");
    	EdbgOutputDebugString("\nStylus Down, please...... \n");

	while(!GetKey())
	{
		if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_DOWN)
		{
			while( ADCTS_GetXPStylusIntState() == STYLUS_DOWN )
			{
				ADC_InitADCCON();
				ADCTS_SetMeasureMode(eADCTS_MEASURE_AUTO_SEQ);

				ADC_EnableStart(ENABLE);

				while(! (ADC_IsAfterStartup() )) ;	//	Wait for begin sampling
				while (! (ADC_IsEOCFlag()));		//	Wait for the EOC

				uXDat	= ADCTS_GetXPData();
				uYDat	= ADCTS_GetYPData();

				EdbgOutputDebugString("X : %d, Y : %d\n",uXDat,uYDat);
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
				ADCTS_SetStylusDetectMode(STYLUS_UP);
				Delay(300);
			}
		}
		else if (g_oADCTSInform.ucTouchStatus == ADCTS_TOUCH_UP)
		{
			if ( ADCTS_GetXPStylusIntState() == STYLUS_UP )
			{
				ADCTS_SetMeasureMode(eADCTS_MEASURE_FOR_STYLUS);
			}
		}
	}

	INTC_Disable(NUM_PENDN);
	EdbgOutputDebugString("\n\n[ADCTS touch screen Tracking test End.]\n");	
}

//////////
// Function Name : ADC_CheckADCLinearity
// Function Description : This function tests ADC Conversion Linearity.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADC_CheckADCLinearity(void)
{
	unsigned char	ucADCChannelCnt 	= 0;
	unsigned char	ucLoopCnt			= 0;
	ULONG	uTotADCValue		= 0;

	EdbgOutputDebugString("The ADC_IN are adjusted to the following values.\n");        
	EdbgOutputDebugString("Push any key to exit!!!\n");    
	EdbgOutputDebugString("ADC conv. freq.=%d(Hz)\n",(ULONG)(g_PCLK/(ADCTS_PRSCVL+1.)));

	while(GetKey()==0) 
	{
		ucLoopCnt 	 = 0;
		uTotADCValue	 = 0;

		ADC_InitADCCON();
		
		EdbgOutputDebugString("\n Input ADC Channel No.(0~7)");
	 	ucADCChannelCnt = UART_GetIntNum(); 

		if ( ucADCChannelCnt > 7 )
			break;

		while(ucLoopCnt < 100)
		{
			ADC_SelectADCCh(ucADCChannelCnt);	//	Channel setup
			ADC_EnableStart(ENABLE);

			while (! ADC_IsAfterStartup() ) ;	//	Wait for begin sampling

			while (! (ADC_IsEOCFlag()));			//	Wait for the EOC

			uTotADCValue += ADCTS_GetXPData();
			ucLoopCnt++;
		}

		EdbgOutputDebugString("ucLoopCnt : %d, Ain%d: %04d \n",ucLoopCnt, ucADCChannelCnt, (ULONG)(uTotADCValue/ucLoopCnt));
	}
}

/*---------------------------------- ISR Routines ---------------------------------*/
//////////
// Function Name : ADC_NormalISR
// Function Description : This function is a ADC interrupt handler of ADC_ConvertADCNormallyInADCInterrupt().
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADC_NormalISR(void)
{
	g_oADCTSInform.uADCSampleData 	= ADCTS_GetXPData();
	g_oADCTSInform.ucADCflag		= 1;

	ADCTS_SetADCClearInt();
	INTC_ClearVectAddr();
}

//////////
// Function Name : ADCTS_StylusCheckISR
// Function Description : This function is a pen-down interrupt handler of ADCTS_StylusCheckInPENDNInterrupt().
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADCTS_StylusCheckISR(void)
{
//	ULONG temp = 0;

//	temp = Inp32(0x71300008);
//	EdbgOutputDebugString("VIC1RawINTR:0x%x\n", temp);
	EdbgOutputDebugString("&");

	ADCTS_SetADCClearWKInt();
	INTC_ClearVectAddr();
}

//////////
// Function Name : ADCTS_StylusTrackingISR
// Function Description : This function is a pen-down interrupt handler of ADCTS_StylusTrackingUsingAutoConversionInPENDNInterruptADCPolling().
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADCTS_StylusTrackingISR(void)
{
	if ( ADCTS_GetXPStylusIntState() == STYLUS_DOWN )
	{
		g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_DOWN;
	}
	else 
	{
		g_oADCTSInform.ucTouchStatus = ADCTS_TOUCH_UP;
	}
	ADCTS_SetADCClearInt();
	ADCTS_SetADCClearWKInt();
	INTC_ClearVectAddr();
}

/*---------------------------------- APIs of rADCCON ---------------------------------*/
//////////
// Function Name : ADC_InitADCCON
// Function Description : This function initializes an ADCCON register.
// Input : 	NONE 
// Output : 	NONE
// Version : v0.1
void ADC_InitADCCON(void)
{
	ULONG			uADCCONValue=0;

	uADCCONValue			= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue			= (1<<14)|(ADCTS_PRSCVL<<6)|(0<<2)|(0<<1);;
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue);	
}

//////////
// Function Name : ADC_IsEOCFlag
// Function Description : This function requests the result of ADC Conversion.
// Input : 	NONE 
// Output : 	TRUE	- End of A/D conversion
//			FALSE 	- A/D conversion in process
// Version : v0.1
unsigned char ADC_IsEOCFlag(void)
{
	ULONG			uADCCONValue=0;
	unsigned char			ucEOCFlag=0;

	uADCCONValue			= ADCTS_GetRegValue(eADCTS_ADCCON);
	ucEOCFlag 				= (uADCCONValue & 0x8000) >> 15;

	if ( ucEOCFlag == 1 )
	{
		return true;
	}
	else 
	{
		return false;
	}
}

//////////
// Function Name : ADC_EnablePrescaler
// Function Description : This function enables ADC prescaler.
// Input : 	ucSel	- Enable/Disable Selector
//						0 : Disable
//						1 : Enable
// Output : 	NONE
// Version : v0.1
void ADC_EnablePrescaler(unsigned char ucSel)
{
	ULONG			uADCCONValue=0;

	uADCCONValue			= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue			|= (uADCCONValue & ~(1<<14))|(ucSel<<12);
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue);	
}

//////////
// Function Name : ADC_SetPrescaler
// Function Description : This function configures the value of ADC prescaler.
// Input : 	ucPrscvl	- ADC prescaler value (5~255)
// Output : 	NONE
// Version : v0.1
void ADC_SetPrescaler(unsigned char ucPrscvl)
{
	ULONG			uADCCONValue=0;

	uADCCONValue			= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue			|= (uADCCONValue & ~(0xff<<6))|(ucPrscvl<<6);
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue);
}

//////////
// Function Name : ADC_SelectADCCh
// Function Description : This function chooses one channel of 8 ADC channels.
// Input : 	ucSel	- number of ADC channel
// Output : 	NONE
// Version : v0.1
void ADC_SelectADCCh(unsigned char ucSel)
{
	ULONG			uADCCONValue=0;

	uADCCONValue			= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue			|= (uADCCONValue & ~(0x7<<3))|(ucSel<<3);
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue);
}

//////////
// Function Name : ADC_SelectStandbyMode
// Function Description : This function chooses 'Standby mode'.
// Input : 	ucSel	- Standby mode select value
//						0 : Normal operation mode
//						1 : Standby mode
// Output : 	NONE
// Version : v0.1
void ADC_SelectStandbyMode(unsigned char ucSel)
{
	ULONG			uADCCONValue=0;

	uADCCONValue			= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue			|= (uADCCONValue & ~(1<<2))|(ucSel<<2);
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue);
}

//////////
// Function Name : ADC_EnableReadStart
// Function Description : This function enables ADC start by read.
// Input : 	ucSel	- Enable/Disable Selector
//						0 : Disable start by read operation
//						1 : Enable start by read operation
// Output : 	NONE
// Version : v0.1
void ADC_EnableReadStart(unsigned char ucSel)
{
	ULONG			uADCCONValue=0;

	uADCCONValue			= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue			|= (uADCCONValue & ~(1<<1))|(ucSel<<1);
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue);
}

//////////
// Function Name : ADC_EnableStart
// Function Description : This function enables ADC start by enable.
//					  If READ_START bit is enabled, this value is not valid.
// Input : 	ucSel	- Enable/Disable Selector
//						0 : No operation
//						1 : Enable(A/D conversion starts and this bit is cleared automatically after the start-up.)
// Output : 	NONE
// Version : v0.1
void ADC_EnableStart(unsigned char ucSel)
{
	ULONG			uADCCONValue=0;

	uADCCONValue			= ADCTS_GetRegValue(eADCTS_ADCCON);
	uADCCONValue			|= (uADCCONValue & ~(1<<0))|(ucSel<<0);
	ADCTS_SetRegValue(eADCTS_ADCCON, uADCCONValue);
}

//////////
// Function Name : ADC_IsAfterStartup
// Function Description : This function requests the result of ADC Enable Start.
// Input : 	NONE 
// Output : 	TRUE	- ADC Enable Started.(ENABLE_START bit of rADCCON is 0)
//			FALSE 	- A/D conversion starts in process
// Version : v0.1
BOOL ADC_IsAfterStartup(void)
{
	ULONG			uADCCONValue=0;
	unsigned char			ucEnableStartFlag=0;

	uADCCONValue			= ADCTS_GetRegValue(eADCTS_ADCCON);
	ucEnableStartFlag			= (uADCCONValue & 0x1) ;

	if ( ucEnableStartFlag == 0 )
	{
		return true;
	}
	else 
	{
		return false;
	}
}
/*---------------------------------- APIs of rADCTSC ---------------------------------*/
//////////
// Function Name : ADCTS_SetMeasureMode
// Function Description : This function chooses the measurement mode of TouchScreen's position.
// Input : 	ucMeasureMode - X/Y Position Measurement mode of TouchScreen
// Output : 	NONE
// Version : v0.1
void ADCTS_SetMeasureMode(unsigned char ucMeasureMode)
{
	ULONG			uADCTSCValue=0;

	uADCTSCValue	= ADCTS_GetRegValue(eADCTS_ADCTSC);
	
	switch(ucMeasureMode)
	{
		case eADCTS_MEASURE_NOP :
			break;
		case eADCTS_MEASURE_MANUAL_X :
			uADCTSCValue	=	0x68;
			break;
		case eADCTS_MEASURE_MANUAL_Y :
			uADCTSCValue	=	0x98;
			break;
		case eADCTS_MEASURE_AUTO_SEQ :
			uADCTSCValue	=	0x04;
			break;
		case eADCTS_MEASURE_FOR_STYLUS :
			uADCTSCValue	=	0xd3;
			break;
	}

	ADCTS_SetRegValue(eADCTS_ADCTSC, uADCTSCValue);
}

//////////
// Function Name : ADCTS_SetStylusDetectMode
// Function Description : This function select the method of  the result of ADC Enable Start.
// Input : 	ucMeasureMode - X/Y Position Measurement mode of TouchScreen
// Output : 	NONE
// Version : v0.1
void ADCTS_SetStylusDetectMode(unsigned char ucStylusDetectMode)
{
	ULONG			uADCTSCValue=0;

	uADCTSCValue	= ADCTS_GetRegValue(eADCTS_ADCTSC);
	uADCTSCValue	|= (uADCTSCValue& ~(1<<8))|(ucStylusDetectMode<<8);

	ADCTS_SetRegValue(eADCTS_ADCTSC, uADCTSCValue);
}

/*---------------------------------- APIs of rADCDLY ---------------------------------*/
//////////
// Function Name : ADCTS_SetDelayClkSrc
// Function Description : This function chooses the source of delay Clock.
// Input : 	uClkSrc - Clock Source Selection Number
//				0 - External input clock
//				1 - RTC clock
// Output : 	NONE
// Version : v0.1
void ADCTS_SetDelayClkSrc(unsigned char uClkSrc)
{
	ULONG			uADCDLYValue=0;

	uADCDLYValue	= ADCTS_GetRegValue(eADCTS_ADCDLY);
	uADCDLYValue	|= (uADCDLYValue& ~(1<<16))|(uClkSrc<<16);

	ADCTS_SetRegValue(eADCTS_ADCDLY, uADCDLYValue);	
}

//////////
// Function Name : ADCTS_SetDelay
// Function Description : This function sets ADC start or interval delay.
// Input : 	uDelayNum - Clock Source Selection Number
// Output : 	NONE
// Version : v0.1
void ADCTS_SetDelay(ULONG uDelayNum)
{
	ULONG			uADCDLYValue=0;

	uADCDLYValue	= ADCTS_GetRegValue(eADCTS_ADCDLY);
	uADCDLYValue	= (uADCDLYValue& ~(0xffff))|(uDelayNum&0xffff);

	ADCTS_SetRegValue(eADCTS_ADCDLY, uADCDLYValue);	
}

/*---------------------------------- APIs of rADCDATx ---------------------------------*/
//////////
// Function Name : ADCTS_GetXPStylusIntState
// Function Description : This function gets the state of Stylus interrupt (Up/Down).
// Input : 	NONE
// Output : 	ucXPState - Stylus state of XP
// Version : v0.1
unsigned char ADCTS_GetXPStylusIntState(void)
{
	ULONG			uADCDAT0Value=0;
	unsigned char			ucXPState=0;

	uADCDAT0Value			= ADCTS_GetRegValue(eADCTS_ADCDAT0);
	ucXPState				= (uADCDAT0Value& (1<<15)) >> 15;

	return ucXPState;
}

//////////
// Function Name : ADCTS_GetYPStylusIntState
// Function Description : This function gets the state of Stylus interrupt (Up/Down).
// Input : 	NONE
// Output : 	ucYPState - Stylus state of YP
// Version : v0.1
unsigned char ADCTS_GetYPStylusIntState(void)
{
	ULONG			uADCDAT1Value=0;
	unsigned char			ucYPState=0;

	uADCDAT1Value			= ADCTS_GetRegValue(eADCTS_ADCDAT1);
	ucYPState				= (uADCDAT1Value& (1<<15)) >> 15;

	return ucYPState;
}

//////////
// Function Name : ADCTS_GetXPData
// Function Description : This function gets ADC Conversion data of X-Position.
// Input : 	NONE
// Output : 	(uADCDAT0Value & 0x3ff) - X-Position Conversiondata value(0~3FF)
// Version : v0.1
ULONG ADCTS_GetXPData(void)
{
	ULONG			uADCDAT0Value=0;

	uADCDAT0Value	= ADCTS_GetRegValue(eADCTS_ADCDAT0);

	return (uADCDAT0Value & 0x3ff);
}

//////////
// Function Name : ADCTS_GetYPData
// Function Description : This function gets ADC Conversion data of Y-Position.
// Input : 	NONE
// Output : 	(uADCDAT1Value & 0x3ff) - Y-Position Conversiondata value(0~3FF)
// Version : v0.1
ULONG ADCTS_GetYPData(void)
{
	ULONG			uADCDAT1Value=0;

	uADCDAT1Value	= ADCTS_GetRegValue(eADCTS_ADCDAT1);

	return (uADCDAT1Value & 0x3ff);
}

/*---------------------------------- APIs of INT Clear Registers ---------------------------------*/
//////////
// Function Name : ADCTS_SetADCClearInt
// Function Description : This function clears ADC Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void ADCTS_SetADCClearInt(void)
{
	ULONG			uADCCLRINTValue=0;

	uADCCLRINTValue	= ADCTS_GetRegValue(eADCTS_ADCCLRINT);
	uADCCLRINTValue	|= 0;
	ADCTS_SetRegValue(eADCTS_ADCCLRINT, uADCCLRINTValue);	
}

//////////
// Function Name : ADCTS_SetADCClearWKInt
// Function Description : This function clears ADCTS WakeUp Interrupt.
// Input : 	NONE
// Output : 	NONE
// Version : v0.1
void ADCTS_SetADCClearWKInt(void)
{
	ULONG			uADCCLRWKValue=0;

	uADCCLRWKValue	= ADCTS_GetRegValue(eADCTS_ADCCLRWK);
	uADCCLRWKValue	|= 0;
	ADCTS_SetRegValue(eADCTS_ADCCLRWK, uADCCLRWKValue);	
}

/*---------------------------------- APIs of general ADCTS ---------------------------------*/
//////////
// Function Name : ADCTS_GetRegValue
// Function Description : This function gets the value of ADCTS Registers.
// Input : 	ucADCTSRegId	- the Id of ADCTS Registers
// Output : 	*puADCTSReg	- the value of specified register
// Version : v0.1
ULONG ADCTS_GetRegValue(unsigned char ucADCTSRegId)
{
	volatile ULONG *	puADCTSBaseAddr;
	volatile ULONG *	puADCTSReg;	

	puADCTSBaseAddr			= &(ADCTS->rADCCON);
	puADCTSReg				= puADCTSBaseAddr + ucADCTSRegId;

	return *puADCTSReg;	
}

//////////
// Function Name : ADCTS_SetRegValue
// Function Description : This function sets the value of ADCTS Registers.
// Input : 	ucADCTSRegId	- the Id of ADCTS Registers
//			uValue			- the value of register
// Output : 	NONE
// Version : v0.1
void ADCTS_SetRegValue(unsigned char ucADCTSRegId, ULONG uValue)
{
	volatile ULONG *	puADCTSBaseAddr;
	volatile ULONG *	puADCTSReg;	

	puADCTSBaseAddr		= &(ADCTS->rADCCON);
	puADCTSReg			= puADCTSBaseAddr + ucADCTSRegId;

	*puADCTSReg		= uValue;
}

