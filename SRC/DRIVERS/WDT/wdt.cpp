
#include <windows.h>
//#include <ceddk.h>
#include <nkintr.h>
//#include <oalintr.h>
#include <pm.h>
#include "pmplatform.h"
#include "Pkfuncs.h"
#include "s3c6410.h"

#include "wdt.h"

// WTCON - control register, bit specifications
#define WTCON_PRESCALE(x)        (((x)&0xff)<<8)    // bit 15:8, prescale value, 0 <= (x) <= 27
#define WTCON_ENABLE            (1<<5)            // bit 5, enable watchdog timer
#define WTCON_CLK_DIV16        (0<<3)
#define WTCON_CLK_DIV32        (1<<3)
#define WTCON_CLK_DIV64        (2<<3)
#define WTCON_CLK_DIV128        (3<<3)
#define WTCON_INT_ENABLE        (1<<2)
#define WTCON_RESET            (1<<0)

// WTCNT - watchdog counter register
#define WTCNT_CNT(x)            ((x)&0xffff)
// WTDAT - watchdog reload value register
#define WTDAT_CNT(x)            ((x)&0xffff)
// WTCLRINT - watchdog interrupt clear register
#define WTCLRINT_CLEAR            (1<<0)
// Watchdog Clock
// PCLK : 25MHz
// PCLK/PRESCALER : 25/25 = 1MHz
// PCLK/PRESCALER/DIVIDER : 1MHz/128 = 7.812 KHz
// MAX Counter = 0xffff = 65535
// Period = 65535/7812 =~ 8.4 sec
#define WD_PRESCALER            (25-1)
#define WD_REFRESH_PERIOD        3000    // tell the OS to refresh watchdog every 3 second.

#define WT_PRESCALER    (0x00)
#define WT_CLK_DIVISION (0x01)

/*****************************************************************************/
/*                                                                           */
/* NAME                                                                      */
/*      WatchTimerStart                                                      */
/* DESCRIPTION                                                               */
/*      This function starts watchdog timer                                  */
/* PARAMETERS                                                                */
/*      none                                                                 */
/* RETURN VALUES                                                             */
/*      none                                                                 */
/* NOTES                                                                     */
/*      This function should be called WatchTimerStop() with a pair          */
/*****************************************************************************/
static void
WatchTimerStart(void)
{
    WATCHreg *pstWDog = (WATCHreg *) WATCH_BASE;
    
    pstWDog->rWTCON = (WT_PRESCALER << 8) |     /* The prescaler value => 63 + 1    */
                      (0x00 << 6) |             /* Reserved                         */
                      (0x00 << 5) |             /* Timer enable or disable          */
                      (WT_CLK_DIVISION << 3) |  /* The clock division factor => 32  */
                      (0x00 << 2) |             /* Disable bit of interrupt         */
                      (0x00 << 1) |             /* Reserved                         */
                      (0x00 << 0);              /* Disable the reset function of timer */
    pstWDog->rWTDAT = 0xFFFF;
    pstWDog->rWTCNT = 0xFFFF;
    pstWDog->rWTCON = (WT_PRESCALER << 8) |     /* The prescaler value => 63 + 1    */
                      (0x00 << 6) |             /* Reserved                         */
                      (0x01 << 5) |             /* Timer enable or disable          */
                      (WT_CLK_DIVISION << 3) |  /* The clock division factor => 32  */
                      (0x00 << 2) |             /* Disable bit of interrupt         */
                      (0x00 << 1) |             /* Reserved                         */
                      (0x00 << 0);              /* Disable the reset function of timer */
}


static BOOL FeedIt()
{
	BOOL ret = FALSE;
	volatile S3C6410_WATCHDOG_REG	*s3c6410WDT = 0;
	do {
		s3c6410WDT = (volatile S3C6410_WATCHDOG_REG *) VirtualAlloc(0,sizeof(S3C6410_WATCHDOG_REG),MEM_RESERVE, PAGE_NOACCESS);
		if (s3c6410WDT == 0) {
			break;
		}

		if (!VirtualCopy((PVOID)s3c6410WDT,(PVOID)(S3C6410_BASE_REG_PA_WATCHDOG),sizeof(S3C6410_WATCHDOG_REG),PAGE_READWRITE | PAGE_NOCACHE )) {
			break;
		}

		ret = TRUE;
	} while (0);

	
	if (ret) {
    s3c6410WDT->rWTCON = (WT_PRESCALER << 8) |     /* The prescaler value => 63 + 1    */
                      (0x00 << 6) |             /* Reserved                         */
                      (0x00 << 5) |             /* Timer enable or disable          */
                      (WT_CLK_DIVISION << 3) |  /* The clock division factor => 32  */
                      (0x00 << 2) |             /* Disable bit of interrupt         */
                      (0x00 << 1) |             /* Reserved                         */
                      (0x01 << 0);              /* Disable the reset function of timer */
    s3c6410WDT->rWTDAT = 0xFFFF;
    s3c6410WDT->rWTCNT = 0xFFFF;
    s3c6410WDT->rWTCON = (WT_PRESCALER << 8) |     /* The prescaler value => 63 + 1    */
                      (0x00 << 6) |             /* Reserved                         */
                      (0x01 << 5) |             /* Timer enable or disable          */
                      (WT_CLK_DIVISION << 3) |  /* The clock division factor => 32  */
                      (0x00 << 2) |             /* Disable bit of interrupt         */
                      (0x00 << 1) |             /* Reserved                         */
                      (0x01 << 0);              /* Disable the reset function of timer */
	}


	if (s3c6410WDT) {
		VirtualFree((void*)s3c6410WDT, sizeof(S3C6410_WATCHDOG_REG), MEM_RELEASE);
		s3c6410WDT = 0;
	}

	return ret;
}

BOOL WINAPI  
DllEntry(HANDLE	hinstDLL, 
			DWORD dwReason, 
			LPVOID  Reserved/* lpvReserved */)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DEBUGREGISTER((HINSTANCE)hinstDLL);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
#ifdef UNDER_CE
	case DLL_PROCESS_EXITING:
		break;
	case DLL_SYSTEM_STARTED:
		break;
#endif
	}

	return TRUE;
}


BOOL WDT_Deinit(DWORD hDeviceContext)
{
	return TRUE;
}
 

BOOL WDT_IOControl(DWORD hOpenContext, 
				   DWORD dwCode, 
				   PBYTE pBufIn, 
				   DWORD dwLenIn, 
				   PBYTE pBufOut, 
				   DWORD dwLenOut, 
				   PDWORD pdwActualOut)
{
 
	return 0;
} 

void WDT_PowerDown(DWORD hDeviceContext)
{
} 

void WDT_PowerUp(DWORD hDeviceContext)
{
} 

DWORD WDT_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	return 0;
} 

DWORD WDT_Seek(DWORD hOpenContext, long Amount, DWORD Type)
{
	return 0;
} 

DWORD WDT_Init(DWORD dwContext)
{
	RETAILMSG(1,(TEXT("WDT: Init\r\n")));
	return TRUE;
}

BOOL WDT_Close(DWORD hOpenContext)
{
	RETAILMSG(1,(TEXT("WDT: Close\r\n")));
	return TRUE;
} 

// Beaf is here

DWORD WDT_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	RETAILMSG(1,(TEXT("WDT: Open\r\n")));
	FeedIt();
	return TRUE;
} 

DWORD WDT_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	RETAILMSG(1,(TEXT("WDT: Write\r\n")));
	FeedIt();
	return TRUE;
}
