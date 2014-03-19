
#include <windows.h>
#include <nkintr.h>
#include <pm.h>
#include "pmplatform.h"
#include "Pkfuncs.h"
#include "BSP.h"

#include "watchdog.h"



#if DEBUG

DBGPARAM dpCurSettings =
{
    TEXT("WDG"),
    {
        TEXT("Errors"),
		TEXT("Warnings"),
		TEXT("Function"),
		TEXT("Init"),
         TEXT("Info"),
		 TEXT("Ist"),
		 TEXT("Undefined"),
		 TEXT("Undefined"),
         TEXT("Undefined"),
		 TEXT("Undefined"),
		 TEXT("Undefined"),
		 TEXT("Undefined"),
         TEXT("Undefined"),
		 TEXT("Undefined"),
		 TEXT("Undefined"),
		 TEXT("Undefined")
    },
        (1 << 0)   // Errors
    |   (1 << 1)   // Warnings
	|	(1 << 5)   // refresh the dog
};
#endif



volatile S3C6410_WATCHDOG_REG *v_pWDGregs ;

bool InitializeAddresses(VOID)
{
	bool	RetValue = TRUE;

	/* WatchDog Register Allocation */
	v_pWDGregs = (volatile S3C6410_WATCHDOG_REG *)VirtualAlloc(0, sizeof(S3C6410_WATCHDOG_REG), MEM_RESERVE, PAGE_NOACCESS);
	if (v_pWDGregs == NULL) 
	{
		DEBUGMSG(ZONE_ERROR,(TEXT("WDG: For IOPregs : VirtualAlloc failed in WDG!\r\n")));
		RetValue = FALSE;
	}
	else 
	{
		if (!VirtualCopy((PVOID)v_pWDGregs, (PVOID)(S3C6410_BASE_REG_PA_WATCHDOG >> 8), sizeof(S3C6410_WATCHDOG_REG), PAGE_PHYSICAL | PAGE_READWRITE | PAGE_NOCACHE)) 
		{
			DEBUGMSG(ZONE_ERROR,(TEXT("WDG: For IOPregs: VirtualCopy failed in WDG!\r\n")));
			RetValue = FALSE;
		}
	}

	if (!RetValue) 
	{
		if (v_pWDGregs) 
		{
			VirtualFree((PVOID) v_pWDGregs, 0, MEM_RELEASE);
		}

		v_pWDGregs = NULL;
	}

	return(RetValue);

}

static BOOL FeedIt()
{


	v_pWDGregs->WTCON = 0x8021; // Init Value
	v_pWDGregs->WTDAT = 0xFEA9;
	v_pWDGregs->WTCNT = 0xFEA9;
	v_pWDGregs->WTCON = ( 1 << 0) | (0x03 << 3 ) | (1 << 5) | (0x5A << 8);
						// reset   // clk div 128 // enable // prescaler value



	return TRUE;
}

BOOL WINAPI  
DllEntry(HMODULE hinstDLL, DWORD dwReason, LPVOID  Reserved/* lpvReserved */)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DEBUGREGISTER(hinstDLL);
		DEBUGMSG (ZONE_INIT, (TEXT("process attach\r\n")));
		DisableThreadLibraryCalls((HMODULE) hinstDLL);
		break;
	case DLL_PROCESS_DETACH:
		DEBUGMSG(ZONE_INIT,(TEXT("WDG: DLL_PROCESS_DETACH\r\n")));
		break;

	}
	return TRUE;
}


BOOL WDG_Deinit(DWORD hDeviceContext)
{
	return TRUE;
}
 

BOOL WDG_IOControl(DWORD hOpenContext, DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn, PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	return 0;
}

void WDG_PowerDown(DWORD hDeviceContext)
{
}

void WDG_PowerUp(DWORD hDeviceContext)
{
}

DWORD WDG_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	return 0;
}

DWORD WDG_Seek(DWORD hOpenContext, long Amount, DWORD Type)
{
	return 0;
}

DWORD WDG_Init(DWORD dwContext)
{
	if (!InitializeAddresses())
		return (FALSE);
	DEBUGMSG(ZONE_INIT,(TEXT("WDG:-- Init\r\n")));
	return TRUE;
}

BOOL WDG_Close(DWORD hOpenContext)
{
	DEBUGMSG(ZONE_INIT,(TEXT("WDG: Close\r\n")));
	return TRUE;
}

// Beaf is here

DWORD WDG_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	DEBUGMSG(ZONE_FUNCTION,(TEXT("WDG: Open\r\n")));
	FeedIt();
	return TRUE;
}

DWORD WDG_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	DEBUGMSG(ZONE_FUNCTION,(TEXT("WDG: Write\r\n")));
	FeedIt();
	return TRUE;
}
