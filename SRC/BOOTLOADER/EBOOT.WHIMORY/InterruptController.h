#ifndef __INTERRUPTCONTROLLER_H__
#define __INTERRUPTCONTROLLER_H__

#include <windows.h>
#include <halether.h>
#include <bsp.h>
#include <specstrings.h>


// IRQ Exception Handler Address
#define pISR        (*(volatile unsigned *)(DRAM_BASE_PA_START+0x18))        // Virtual Address 0x0 is mapped to 0x50000000, ISR Address is VA 0x18

void InitializeInterrupt(void);
void VIC_setHandler(unsigned intNum, void (*handler)(void) );
void VIC_InterruptEnable(UINT32 intNum);
void VIC_InterruptDisable(UINT32 intNum);



#endif