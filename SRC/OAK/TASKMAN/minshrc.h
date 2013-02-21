//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
/*---------------------------------------------------------------------------*\
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *
 *  file:  MinShRC.h
 *  purpose: Sample shell resource ids
 *
\*---------------------------------------------------------------------------*/

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

// Strings
#define IDS_RUN_ERROR			2002
#define IDS_BROWSE_TITLE		2003
#define IDS_BROWSE_FILTER		2004

// Taskman Dialogs
#define IDD_TASK_MANAGER		106
#define IDD_REALLYKILL          107
#define IDD_RUN					108
#define IDD_TASK_MANAGER_G		109
#define IDD_REALLYKILL_G			110
#define IDD_RUN_G				111

// Constants for Task Manager dialog controls
#define IDC_LISTBOX				1000
#define IDC_ENDTASK				1002
#define IDC_RUN					1003
#define IDC_RUNCMD				1004
#define IDC_BROWSE				1005

// Battery warning dialogs 
#define IDD_BACKUP_LOW      	210
#define IDD_BACKUP_VLOW         211
#define IDD_MAIN_VLOW           212

// Icons used by battery warning dialogs 
#define IDI_VERYLOW             313
#define IDI_LOWBKUP             314
#define IDI_VLOWBKUP            316

#endif /* __RESOURCE_H__ */
