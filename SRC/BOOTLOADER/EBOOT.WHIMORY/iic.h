#ifndef __iic__
#define __iic__

typedef unsigned long    ULONG;
typedef unsigned short    u16;
typedef unsigned char    u8;

#define I2C0_BASE    (0xB2B04000) //(0x7C000000) -> 0x91800000

enum IIC0_SFR			// Channel0, Added by SOP on 2008/03/01
{
	rIICCON0		= I2C0_BASE+0x00,
	rIICSTAT0		= I2C0_BASE+0x04,
	rIICADD0		= I2C0_BASE+0x08,
	rIICDS0			= I2C0_BASE+0x0C,
	rIICLC0			= I2C0_BASE+0x10
};

#define	SlaveRX			(0)
#define	SlaveTX			(1)
#define	MasterRX		(2)
#define	MasterTX		(3)

void Isr_IIC( void);
void IIC_Open(ULONG ufreq);
void IIC_Close(void);

u8 IIC_Status(void);

int IIC_Write(u8 cSlaveAddr,  u8 cAddr, u8 cData);
void IIC_Read(u8 cSlaveAddr,u8 cAddr,u8 *cData);

#endif
