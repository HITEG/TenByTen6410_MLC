#include <bsp.h>
#include "iic.h"
#include "InterruptController.h"


#define Outp32(addr, data)  (*(volatile UINT32 *)(addr) = (data))
#define Outp16(addr, data)  (*(volatile UINT16 *)(addr) = (data))
#define Outp8(addr, data)   (*(volatile UINT8 *)(addr) = (data))
#define Inp32(addr, data)   (data = (*(volatile UINT32 *)(addr)))
#define Inp16(addr, data)   (data = (*(volatile UINT16 *)(addr)))
#define Inp8(addr, data)    (data = (*(volatile UINT16 *)(addr)))
#define Input32(addr) (*(volatile UINT32 *)(addr))
#define Input8(addr)			(*(volatile u8 *)(addr))


/*=================================IIC================================================*/
static volatile u8 *g_PcIIC_BUFFER;
static volatile ULONG g_uIIC_PT;
static ULONG g_uIIC_DATALEN;
static volatile u8 g_cIIC_STAT0;
static volatile u8 g_cIIC_SlaveRxDone;
static volatile u8 g_cIIC_SlaveTxDone;
static volatile ULONG g_IIC_WRITE_TIME;
static volatile ULONG g_IIC_WAIT_TIME;
static unsigned g_PCLK;


static void IIC_SetWrite(u8 cSlaveAddr, u8 * pData, ULONG uDataLen);
static int IIC_Wait(void);
//////////
// Function Name : Isr_IIC
// Function Description : This function is Interrupt Service Routine of IIC
//					  when interrupt occurs, check IICSTAT, find mode and operate it
// Input : NONE
// Output : NONE
// Version : v0.1
extern Delay(unsigned ms);
void Isr_IIC( void)
{
	ULONG uTemp0 = 0;
	u8 cCnt;
	//EdbgOutputDebugString("[Eboot] IIC_ISR\r\n");
	g_cIIC_STAT0 = Input32(rIICSTAT0);
	switch( (g_cIIC_STAT0>>6)&0x3) 
	{
		case SlaveRX	:	
			//OEMWriteDebugString(L"IICSTAT0 = %x	",g_cIIC_STAT0);

			if(g_uIIC_PT<100) 
			{
				g_PcIIC_BUFFER[g_uIIC_PT++]=Input8(rIICDS0);
				Delay(100);
				
				uTemp0 = Input32(rIICCON0);
				uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
				Outp32(rIICCON0,uTemp0);
				break;
			}

			Outp8(rIICSTAT0,0x0);			// Stop Int
			
			uTemp0 = Input32(rIICCON0);
			uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON0,uTemp0);

			g_cIIC_SlaveRxDone = 1;

			Delay(1);						//	wait until Stop Condition is in effect
			break;

		case SlaveTX	:		
			//OEMWriteDebugString(L"IICSTAT = %x	",g_cIIC_STAT0);

			if(g_uIIC_PT>100)
			{
				Outp32(rIICSTAT0,0xd0);			//	Stop Master Tx condition, ACK flag clear

				uTemp0 = Input32(rIICCON0);
				uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
				Outp32(rIICCON0,uTemp0);

				g_cIIC_SlaveTxDone = 1;

				Delay(1);						//	wait until Stop Condition is in effect
				break;
			}

			Outp8(rIICDS0,g_PcIIC_BUFFER[g_uIIC_PT++]);
			for(cCnt=0;cCnt<10;cCnt++);		//	for setup time (rising edge of IICSCL)
			Delay(100);
			
			uTemp0 = Input32(rIICCON0);
			uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON0,uTemp0);				
			break;

		case MasterRX:	
			if (g_uIIC_PT>0)
				g_PcIIC_BUFFER[g_uIIC_PT-1] = Input32(rIICDS0);

			g_uIIC_PT++;

			if (g_uIIC_PT==g_uIIC_DATALEN)
			{
				uTemp0 = Input32(rIICCON0);
				uTemp0 &= ~(1<<7);			// Disable Ack generation
				Outp32(rIICCON0,uTemp0);		// EEPROM 에서는 stop 전에 ACK 안보냄
			}
			else if (g_uIIC_PT > g_uIIC_DATALEN)
				Outp32(rIICSTAT0,0x90);		//	Stop Master Rx condition

			uTemp0 = Input32(rIICCON0);
			uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON0,uTemp0);
			break;

		case MasterTX:		

			if (g_uIIC_PT<g_uIIC_DATALEN)
				Outp32(rIICDS0,g_PcIIC_BUFFER[g_uIIC_PT]);
			else	
				Outp32(rIICSTAT0,0xd0);			//	Stop Master Tx condition, ACK flag clear

			g_uIIC_PT++;
			uTemp0 = Input32(rIICCON0);
			uTemp0 &= ~(1<<4);			//	Clear pending bit to resume
			Outp32(rIICCON0,uTemp0);
			break;
	}

	g_cIIC_STAT0	&=0xf;
}
//////////
// Function Name : IIC_Open
// Function Description : This function Set up VIC & IICCON with user's input frequency which determines uClkValue
// Input : ufreq	ufreq(Hz) = PCLK/16/uClkValue
// Output : NONE
// Version : v0.1
void IIC_Open( ULONG ufreq)		//	Hz order. freq(Hz) = PCLK/16/clk_divider
{
	ULONG	uSelClkSrc;
	ULONG	uClkValue;

	
	//INTC_SetVectAddr(NUM_IIC,Isr_IIC);
	//INTC_Enable(NUM_IIC);
    volatile S3C6410_GPIO_REG *pGPIOReg = (S3C6410_GPIO_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);

	g_PCLK=System_GetPCLK();

	pGPIOReg->GPBCON &= ~((0xf<<20)|(0xf<<24));
	pGPIOReg->GPBCON |= ((0x2<<20)|(0x2<<24));
	pGPIOReg->GPBPUDSLP &= ~((0x3<<10)|(0x3<<12));
	pGPIOReg->GPBPUDSLP |= ((0x2<<10)|(0x2<<12));

    VIC_InterruptEnable(PHYIRQ_I2C);
	VIC_setHandler(PHYIRQ_I2C,Isr_IIC);
		
	if ((((g_PCLK>>4)/ufreq)-1)>0xf) 
	{
		uSelClkSrc	=	1;
		uClkValue	=	((g_PCLK>>9)/ufreq) -1;		//	PCLK/512/freq
	} 
	else 
	{
		uSelClkSrc	=	0;
		uClkValue	=	((g_PCLK>>4)/ufreq) -1;		//	PCLK/16/freq
	}

	//Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16


	EdbgOutputDebugString("g_PCLK = %d",g_PCLK);
	EdbgOutputDebugString("uSelClkSrc = %d",uSelClkSrc);
	EdbgOutputDebugString("uClkValue = %d",uClkValue);


	Outp32(rIICCON0,(uSelClkSrc<<6) | (1<<5) | (uClkValue&0xf));
	Outp32(rIICADD0,0xc0);		//	Slave address = [7:1]
	Outp32(rIICSTAT0,0x10);	//	IIC bus data output enable(Rx/Tx)
	Outp32(rIICLC0,0x0);			//	SDA Filter enable,delayed 15clks
}

//////////
// Function Name : IIC_Close
// Function Description : This function disable IIC
// Input : NONE
// Output : NONE
// Version : v0.1
void IIC_Close(void)
{
    volatile S3C6410_GPIO_REG *pGPIOReg = (S3C6410_GPIO_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);

	pGPIOReg->GPBCON &= ~((0xf<<20)|(0xf<<24));
	pGPIOReg->GPBPUDSLP &= ~((0x3<<10)|(0x3<<12));
	
	//INTC_Disable(NUM_IIC);
    VIC_InterruptDisable(PHYIRQ_I2C);
	
	Outp32(rIICSTAT0,0x0);                		//	IIC bus data output disable(Rx/Tx)
	//GPIO_SetFunctionEach(eGPIO_B,eGPIO_5,0);
	//GPIO_SetFunctionEach(eGPIO_B,eGPIO_6,0);	
	//GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_5,0);
	//GPIO_SetPullUpDownEach(eGPIO_B,eGPIO_6,0);
}


//////////
// Function Name : IIC_SetWrite
// Function Description : This function sets up write mode with 7-bit addresses
// Input :  cSlaveAddr [8bit SlaveDeviceAddress], 
//		  pData[Data which you want to write], 
//		  uDataLen [Data Length]
// Output : NONE
// Version : v0.1

static void IIC_SetWrite( u8 cSlaveAddr,  u8 *pData, ULONG uDataLen)
{
	ULONG uTmp0;
	ULONG uTmp1;
	
	uTmp0 = Input32(rIICSTAT0);
	while(uTmp0&(1<<5))		//	Wait until IIC bus is free.
		uTmp0 = Input32(rIICSTAT0);			

	g_PcIIC_BUFFER	=	pData;
	g_uIIC_PT		=	0;
	g_uIIC_DATALEN	=	uDataLen;

	uTmp1 = Input32(rIICCON0);
	uTmp1 |= (1<<7);
	Outp32(rIICCON0,uTmp1);				//	Ack generation Enable
	
	Outp32(rIICDS0,cSlaveAddr);
	Outp32(rIICSTAT0,0xf0);				//	Master Tx Start.
}



static int IIC_WaitTimeOut()
{
	g_IIC_WAIT_TIME++;
	if(g_IIC_WAIT_TIME%50000)
		return 0;
	return 1;
}

static int IIC_WriteTimeOut()
{
	g_IIC_WRITE_TIME++;
	if(g_IIC_WRITE_TIME%300)
		return 0;
	return 1;
}
//////////
// Function Name : IIC_Wait
// Function Description : This function waits until the command takes effect
//											But not for IIC bus free
// Input : NONE 
// Output : NONE
// Version : v0.1
static int IIC_Wait( void)						//	Waiting for the command takes effect.
{
	g_IIC_WAIT_TIME=1;
	while(g_uIIC_PT<=g_uIIC_DATALEN){
		if(IIC_WaitTimeOut())
		{
			EdbgOutputDebugString("[Eboot] IIC_WaitTimeOut!\r\n");
			return 0;
		}
	}
	return 1;
}

//////////
// Function Name : IIC_Status
// Function Description : This function returns IIC Status Register value at last interrupt occur
// Input : NONE
// Output : NONE
// Version : v0.1
u8 IIC_Status( void)						//	Return IIC Status Register value at last interrupt occur.
{
	return	g_cIIC_STAT0;
}
//////////
// Function Name : IIC_Write
// Function Description : This function STARTs up write mode with 7-bit addresses
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		 cAddr[8bit Address where you want to write], 
//		 pData[Data which you want to write]
// Output : NONE
// Version : v0.1

int IIC_Write(u8 cSlaveAddr, u8 cAddr, u8 cData)
{
	u8 cD[2];

	cD[0]=cAddr;
	cD[1]=cData;

	g_IIC_WRITE_TIME=1;
	
	IIC_SetWrite(cSlaveAddr, cD, 2);

	if(!IIC_Wait())
		return 0;
	do
	{									//	Polling for an ACK signal from SerialEEPROM.
		IIC_SetWrite(cSlaveAddr, NULL, 0);
		if(!IIC_Wait())
			return 0;
		if(IIC_WriteTimeOut())
		{
			EdbgOutputDebugString("[Eboot] IIC_WriteTimeOut!\r\n");
			return 0;
		}
	} while(IIC_Status()&0x1);
	return 1;
}

//////////
// Function Name : IIC_SetRead
// Function Description : This function sets up Read mode with 7-bit addresses
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		  pData[Data which you want to read], 
//		  uDataLen [Data Length]
// Output : NONE
// Version : v0.1
static void IIC_SetRead(  u8 cSlaveAddr,  u8 *pData, ULONG uDataLen)
{
	ULONG uTmp2;
	ULONG uTmp3;
	
	uTmp2= Input32(rIICSTAT0);
	while(uTmp2&(1<<5))		//	Wait until IIC bus is free.
		uTmp2 = Input32(rIICSTAT0);			

	g_PcIIC_BUFFER	=	pData;
	g_uIIC_PT		=	0;
	g_uIIC_DATALEN	=	uDataLen;

	uTmp3 = Input32(rIICCON0);
	uTmp3 |= (1<<7);
	Outp32(rIICCON0,uTmp3);				//	Ack generation Enable
	Outp32(rIICDS0,cSlaveAddr);
	Outp32(rIICSTAT0,0xB0);				//	Master Rx Start.
}
//////////
// Function Name : IIC_Read
// Function Description : This function STARTs up read mode with 7-bit addresses
// Input : cSlaveAddr [8bit SlaveDeviceAddress], 
//		 cAddr [8bit Address where you want to read], 
//		 cData [pointer of Data which you want to read]
// Output : NONE
// Version : v0.1
void IIC_Read(u8 cSlaveAddr,u8 cAddr,u8 *cData)
{
	IIC_SetWrite( cSlaveAddr, &cAddr, 1);			// following EEPROM random address access procedure
	IIC_SetRead( cSlaveAddr, cData, 1);
	IIC_Wait();								//	Waiting for read complete.
}

