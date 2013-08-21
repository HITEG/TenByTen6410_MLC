/**************************************************************************
 *
 * $Id: wince.c,v 1.1.1.1 2007/04/16 03:45:52 bill Exp $
 *
 * File: Types.h
 *
 * Copyright (c) 2000-2002 Davicom Inc.  All rights reserved.
 *
 *************************************************************************/

#include <ndis.h>

BOOL
DllEntry(
    HANDLE    hInstDll,
    DWORD     dwOp,
    LPVOID    lpvReserved)
{
    switch (dwOp)
    {
    
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls ((HMODULE)hInstDll);
            RETAILMSG(1, (TEXT("DM9000x DLL_PROCESS_ATTACH!!\r\n")));
            break;
            
        case DLL_PROCESS_DETACH:
        
        default:
            RETAILMSG(1, (TEXT("DM9000x DLL_PROCESS_DETACH!!\r\n")));
            break;
    }
    
    return (TRUE);
}

