#ifndef __WDG_H_
#define __WDG_H_

#ifdef __cplusplus
extern "C" {
#endif



#ifdef DEBUG
#ifdef ZONE_ERROR
	#undef ZONE_ERROR
#endif
#define ZONE_ERROR            DEBUGZONE(0)
#ifdef ZONE_WARNING
#undef ZONE_WARNING
#endif
#define ZONE_WARNING            DEBUGZONE(1)
#define ZONE_FUNCTION           DEBUGZONE(2)
#ifdef ZONE_INIT
#undef ZONE_INIT
#endif
#define ZONE_INIT           DEBUGZONE(3)
#define ZONE_INFO           DEBUGZONE(4)
#define ZONE_IST          DEBUGZONE(5)

#define ZONE_TRACE          DEBUGZONE(15)

//
// these should be removed in the code if you can 'g' past these successfully
//
#define TEST_TRAP { \
   NKDbgPrintfW( TEXT("%s: Code Coverage Trap in: WDG, Line: %d\n"), TEXT(__FILE__), __LINE__); \
   DebugBreak();  \
}
#else
#define TEST_TRAP
#endif




DWORD WDG_Init(DWORD dwContext);
DWORD WDG_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode);
DWORD WDG_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes);

DWORD WDG_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count);
DWORD WDG_Seek(DWORD hOpenContext, long Amount, DWORD Type);

BOOL WDG_IOControl(DWORD hOpenContext, DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn, PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut);
BOOL WDG_Close(DWORD hOpenContext);
BOOL WDG_Deinit(DWORD hDeviceContext);
void WDG_PowerUp(DWORD hDeviceContext);
void WDG_PowerDown(DWORD hDeviceContext);

#ifdef __cplusplus
}
#endif


#endif