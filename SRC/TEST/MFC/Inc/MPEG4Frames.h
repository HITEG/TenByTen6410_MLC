//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Use of this sample source code is subject to the terms of the 
// Software License Agreement (SLA) under which you licensed this software product.
// If you did not accept the terms of the license agreement, 
// you are not authorized to use this sample source code. 
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
/////
///   MPEG4Frames.h
///
///   Written by Simon Chun (simon.chun@samsung.com)
///   2007/03/23
///

#ifndef __SAMSUNG_SYSLSI_AP_MPEG4_FRAMES_H__
#define __SAMSUNG_SYSLSI_AP_MPEG4_FRAMES_H__

typedef struct
{
	int  width, height;

} MPEG4_CONFIG_DATA;


#define MPEG4_CODING_TYPE_I		0	// I-frame
#define MPEG4_CODING_TYPE_P		1	// P-frame


#ifdef __cplusplus
extern "C" {
#endif

int ExtractConfigStreamMpeg4(FRAMEX_CTX  *pFrameExCtx, void *fp, unsigned char buf[], int buf_size, MPEG4_CONFIG_DATA *conf_data);
int NextFrameMpeg4(FRAMEX_CTX  *pFrameExCtx, void *fp, unsigned char buf[], int buf_size, unsigned int *coding_type);

#ifdef __cplusplus
}
#endif


#endif /* __SAMSUNG_SYSLSI_AP_MPEG4_FRAMES_H__ */
