#ifndef __WDG_H_
#define __WDG_H_

#ifdef __cplusplus
extern "C" {
#endif



#ifdef DEBUG
#define ZONE_ERR            DEBUGZONE(0)
#define ZONE_WRN            DEBUGZONE(1)
#define ZONE_INIT           DEBUGZONE(2)
#define ZONE_OPEN           DEBUGZONE(3)
#define ZONE_READ           DEBUGZONE(4)
#define ZONE_WRITE          DEBUGZONE(5)
#define ZONE_IOCTL          DEBUGZONE(6)
//...
#define ZONE_TRACE          DEBUGZONE(15)

//
// these should be removed in the code if you can 'g' past these successfully
//
#define TEST_TRAP { \
   NKDbgPrintfW( TEXT("%s: Code Coverage Trap in: Led, Line: %d\n"), TEXT(__FILE__), __LINE__); \
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