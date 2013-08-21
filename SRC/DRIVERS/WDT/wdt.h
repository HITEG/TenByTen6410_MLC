#ifndef __WDT_H_
#define __WDT_H_

#ifdef __cplusplus
extern "C" {
#endif

DWORD WDT_Init(DWORD dwContext);
DWORD WDT_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode);
DWORD WDT_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes);

DWORD WDT_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count);
DWORD WDT_Seek(DWORD hOpenContext, long Amount, DWORD Type);

BOOL WDT_IOControl(DWORD hOpenContext, DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn, PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut);
BOOL WDT_Close(DWORD hOpenContext);
BOOL WDT_Deinit(DWORD hDeviceContext);
void WDT_PowerUp(DWORD hDeviceContext);
void WDT_PowerDown(DWORD hDeviceContext);

#ifdef __cplusplus
}
#endif


#endif