#include <windows.h>

#include "MfcDriver.h"
#include "MfcDrvParams.h"

#include "SsbSipH264Decode.h"
#include "SsbSipLogMsg.h"

#define _MFCLIB_H264_DEC_MAGIC_NUMBER        0x92241002

typedef struct
{
    DWORD   magic;
    HANDLE  hOpen;

    unsigned int     width, height;

    void   *p_buf;
    int     size;

    DWORD   proc_id;    // process ID

    int     fInit;
} _MFCLIB_H264_DEC;



void *SsbSipH264DecodeInit()
{
    _MFCLIB_H264_DEC  *pCTX;
    HANDLE              hOpen;

    //////////////////////////////
    /////     CreateFile     /////
    //////////////////////////////
    hOpen = CreateFile(L"MFC1:",
                       GENERIC_READ|GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);
    if (hOpen == INVALID_HANDLE_VALUE) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeInit", "MFC Open failure\n");
        return NULL;
    }

    pCTX = (_MFCLIB_H264_DEC *) malloc(sizeof(_MFCLIB_H264_DEC));
    if (pCTX == NULL) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeInit", "malloc failed.\n");
        CloseHandle(hOpen);
        return NULL;
    }
    memset(pCTX, 0, sizeof(_MFCLIB_H264_DEC));

    pCTX->magic   = _MFCLIB_H264_DEC_MAGIC_NUMBER;
    pCTX->hOpen   = hOpen;
    pCTX->proc_id = GetCurrentProcessId();
    pCTX->fInit   = 0;


    return (void *) pCTX;
}


int SsbSipH264DecodeExe(void *openHandle, long lengthBufFill)
{
    _MFCLIB_H264_DEC   *pCTX;
    MFC_ARGS            mfc_args;
    BOOL                r;


    ////////////////////////////////
    //  Input Parameter Checking  //
    ////////////////////////////////
    if (openHandle == NULL) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeExe", "openHandle is NULL\n");
        return SSBSIP_H264_DEC_RET_ERR_INVALID_HANDLE;
    }
    if ((lengthBufFill < 0) || (lengthBufFill > 0x100000)) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeExe", "lengthBufFill is invalid. (lengthBufFill=%d)\n", lengthBufFill);
        return SSBSIP_H264_DEC_RET_ERR_INVALID_PARAM;
    }


    pCTX  = (_MFCLIB_H264_DEC *) openHandle;


    if (!pCTX->fInit) {

        /////////////////////////////////////////////////
        /////           (DeviceIoControl)           /////
        /////       IOCTL_MFC_H264_DEC_EXE         /////
        /////////////////////////////////////////////////
        mfc_args.dec_init.in_strmSize = lengthBufFill;
        r = DeviceIoControl(pCTX->hOpen, IOCTL_MFC_H264_DEC_INIT,
                            &mfc_args, sizeof(MFC_DEC_INIT_ARG),
                            NULL, 0,
                            NULL,
                            NULL);
        if ((r == FALSE) || (mfc_args.get_buf_addr.ret_code != 0)) {
            return SSBSIP_H264_DEC_RET_ERR_CONFIG_FAIL;
        }

        // Output argument (width , height)
        pCTX->width  = mfc_args.dec_init.out_width;
        pCTX->height = mfc_args.dec_init.out_height;

        pCTX->fInit = 1;

        return SSBSIP_H264_DEC_RET_OK;
    }


    /////////////////////////////////////////////////
    /////           (DeviceIoControl)           /////
    /////       IOCTL_MFC_H264_DEC_EXE         /////
    /////////////////////////////////////////////////
    mfc_args.dec_exe.in_strmSize = lengthBufFill;
    r = DeviceIoControl(pCTX->hOpen, IOCTL_MFC_H264_DEC_EXE,
                        &mfc_args, sizeof(MFC_DEC_EXE_ARG),
                        NULL, 0,
                        NULL,
                        NULL);
    if ((r == FALSE) || (mfc_args.dec_exe.ret_code != 0)) {
        return SSBSIP_H264_DEC_RET_ERR_DECODE_FAIL;
    }

    return SSBSIP_H264_DEC_RET_OK;
}


int SsbSipH264DecodeDeInit(void *openHandle)
{
    _MFCLIB_H264_DEC  *pCTX;


    ////////////////////////////////
    //  Input Parameter Checking  //
    ////////////////////////////////
    if (openHandle == NULL) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeDeInit", "openHandle is NULL\n");
        return SSBSIP_H264_DEC_RET_ERR_INVALID_HANDLE;
    }


    pCTX  = (_MFCLIB_H264_DEC *) openHandle;


    CloseHandle(pCTX->hOpen);
    free(pCTX);

    return 0;
}


void *SsbSipH264DecodeGetInBuf(void *openHandle, long size)
{
    _MFCLIB_H264_DEC  *pCTX;
    MFC_ARGS           mfc_args;
    BOOL               r;

    void       *pStrmBuf;
    DWORD       nStrmBufSize; 

    ////////////////////////////////
    //  Input Parameter Checking  //
    ////////////////////////////////
    if (openHandle == NULL) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetInBuf", "openHandle is NULL\n");
        return NULL;
    }
    if ((size < 0) || (size > 0x100000)) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetInBuf", "size is invalid. (size=%d)\n", size);
        return NULL;
    }

    pCTX  = (_MFCLIB_H264_DEC *) openHandle;



    /////////////////////////////////////////////////
    /////           (DeviceIoControl)           /////
    /////      IOCTL_MFC_GET_STRM_BUF_ADDR      /////
    /////////////////////////////////////////////////
    mfc_args.get_buf_addr.in_usr_data = pCTX->proc_id;
    r = DeviceIoControl(pCTX->hOpen, IOCTL_MFC_GET_LINE_BUF_ADDR,
                        &mfc_args, sizeof(MFC_GET_BUF_ADDR_ARG),
                        NULL, 0,
                        NULL,
                        NULL);
    if ((r == FALSE) || (mfc_args.get_buf_addr.ret_code != 0)) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetInBuf", "Failed in get LINE_BUF address\n");
        return NULL;
    }

    // Output arguments
    pStrmBuf     = (void *) mfc_args.get_buf_addr.out_buf_addr;    
    nStrmBufSize = mfc_args.get_buf_addr.out_buf_size;

    if ((long)nStrmBufSize < size) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetInBuf", "Requested size is greater than available buffer. (size=%d, avail=%d)\n",
                size, nStrmBufSize);

        return NULL;
    }

    return pStrmBuf;
}


void *SsbSipH264DecodeGetOutBuf(void *openHandle, long *size)
{
    _MFCLIB_H264_DEC  *pCTX;
    MFC_ARGS           mfc_args;
    BOOL               r;

    void       *pFramBuf;
    DWORD       nFramBufSize;

    ////////////////////////////////
    //  Input Parameter Checking  //
    ////////////////////////////////
    if (openHandle == NULL) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetOutBuf", "openHandle is NULL\n");
        return NULL;
    }
    if (size == NULL) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetOutBuf", "size is NULL\n");
        return NULL;
    }

    pCTX  = (_MFCLIB_H264_DEC *) openHandle;



    /////////////////////////////////////////////////
    /////           (DeviceIoControl)           /////
    /////      IOCTL_MFC_GET_STRM_BUF_ADDR      /////
    /////////////////////////////////////////////////
    mfc_args.get_buf_addr.in_usr_data = pCTX->proc_id;
    r = DeviceIoControl(pCTX->hOpen, IOCTL_MFC_GET_FRAM_BUF_ADDR,
                        &mfc_args, sizeof(MFC_GET_BUF_ADDR_ARG),
                        NULL, 0,
                        NULL,
                        NULL);
    if ((r == FALSE) || (mfc_args.get_buf_addr.ret_code != 0)) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetOutBuf", "Failed in get FRAM_BUF address.\n");
        return NULL;
    }

    // Output arguments
    pFramBuf     = (void *) mfc_args.get_buf_addr.out_buf_addr;
    nFramBufSize = mfc_args.get_buf_addr.out_buf_size;

    *size = nFramBufSize;

    return pFramBuf;
}



int SsbSipH264DecodeSetConfig(void *openHandle, H264_DEC_CONF conf_type, void *value)
{
    _MFCLIB_H264_DEC   *pCTX;
    MFC_ARGS            mfc_args;
    BOOL                r;


    ////////////////////////////////
    //  Input Parameter Checking  //
    ////////////////////////////////
    if (openHandle == NULL) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeSetConfig", "openHandle is NULL\n");
        return SSBSIP_H264_DEC_RET_ERR_INVALID_HANDLE;
    }
    if (value == NULL) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeSetConfig", "value is NULL\n");
        return SSBSIP_H264_DEC_RET_ERR_INVALID_PARAM;
    }

    pCTX  = (_MFCLIB_H264_DEC *) openHandle;

    switch (conf_type) {

    case H264_DEC_SETCONF_POST_ROTATE:

        mfc_args.set_config.in_config_param     = MFC_SET_CONFIG_DEC_ROTATE;
        mfc_args.set_config.in_config_value[0]  = *((unsigned int *) value);
        mfc_args.set_config.in_config_value[1]  = 0;
        r = DeviceIoControl(pCTX->hOpen, IOCTL_MFC_SET_CONFIG,
                            &mfc_args, sizeof(MFC_SET_CONFIG_ARG),
                            NULL, 0,
                            NULL,
                            NULL);
        if ((r == FALSE) || (mfc_args.set_config.ret_code != 0)) {
            LOG_MSG(LOG_ERROR, "SsbSipH264DecodeSetConfig", "Error in H264_DEC_SETCONF_POST_ROTATE.\n");
            return SSBSIP_H264_DEC_RET_ERR_SETCONF_FAIL;
        }
        break;

    default:
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeSetConfig", "No such conf_type is supported.\n");
        return SSBSIP_H264_DEC_RET_ERR_SETCONF_FAIL;
    }


    return SSBSIP_H264_DEC_RET_OK;
}




int SsbSipH264DecodeGetConfig(void *openHandle, H264_DEC_CONF conf_type, void *value)
{
    _MFCLIB_H264_DEC   *pCTX;
    MFC_ARGS            mfc_args;
    BOOL                r;


    ////////////////////////////////
    //  Input Parameter Checking  //
    ////////////////////////////////
    if (openHandle == NULL) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetConfig", "openHandle is NULL\n");
        return SSBSIP_H264_DEC_RET_ERR_INVALID_HANDLE;
    }
    if (value == NULL) {
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetConfig", "value is NULL\n");
        return SSBSIP_H264_DEC_RET_ERR_INVALID_PARAM;
    }

    pCTX  = (_MFCLIB_H264_DEC *) openHandle;

    switch (conf_type) {

    case H264_DEC_GETCONF_STREAMINFO:
        ((SSBSIP_H264_STREAM_INFO *)value)->width  = pCTX->width;
        ((SSBSIP_H264_STREAM_INFO *)value)->height = pCTX->height;
        break;

    case H264_DEC_GETCONF_PHYADDR_FRAM_BUF:

        /////////////////////////////////////////////////
        /////           (DeviceIoControl)           /////
        /////      IOCTL_MFC_GET_FRAM_BUF_ADDR      /////
        /////////////////////////////////////////////////
        mfc_args.get_buf_addr.in_usr_data = pCTX->proc_id;
        r = DeviceIoControl(pCTX->hOpen, IOCTL_MFC_GET_PHY_FRAM_BUF_ADDR,
                            &mfc_args, sizeof(MFC_GET_BUF_ADDR_ARG),
                            NULL, 0,
                            NULL,
                            NULL);
        if ((r == FALSE) || (mfc_args.get_buf_addr.ret_code != 0)) {
            LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetConfig", "Error in H264_DEC_GETCONF_PHYADDR_FRAM_BUF.\n");
            return SSBSIP_H264_DEC_RET_ERR_GETCONF_FAIL;
        }

        // Output arguments
        ((unsigned int*) value)[0] = mfc_args.get_buf_addr.out_buf_addr;
        ((unsigned int*) value)[1] = mfc_args.get_buf_addr.out_buf_size;

        break;

    case H264_DEC_GETCONF_FRAM_NEED_COUNT:

        mfc_args.get_config.in_config_param      = MFC_GET_CONFIG_DEC_FRAME_NEED_COUNT;
        mfc_args.get_config.out_config_value[0]  = 0;
        mfc_args.get_config.out_config_value[1]  = 0;
        r = DeviceIoControl(pCTX->hOpen, IOCTL_MFC_GET_CONFIG,
                            &mfc_args, sizeof(MFC_GET_CONFIG_ARG),
                            NULL, 0,
                            NULL,
                            NULL);
        if ((r == FALSE) || (mfc_args.get_config.ret_code != 0)) {
            LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetConfig", "Error in H264_DEC_GETCONF_FRAM_NEED_COUNT.\n");
            return SSBSIP_H264_DEC_RET_ERR_GETCONF_FAIL;
        }

        // Output arguments
        ((unsigned int*) value)[0] = mfc_args.get_config.out_config_value[0];

        break;

    default:
        LOG_MSG(LOG_ERROR, "SsbSipH264DecodeGetConfig", "No such conf_type is supported.\n");
        return SSBSIP_H264_DEC_RET_ERR_GETCONF_FAIL;
    }


    return SSBSIP_H264_DEC_RET_OK;
}


