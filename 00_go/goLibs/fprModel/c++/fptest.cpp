////
//

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <fstream>

// #include <iconv.h>
// #include <sched.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include <unistd.h> //readlink

#include "./sdk/gals.h"
#include "./sdk/gamc.h"
// #include "./cpp-bmp-images/BMP.h"
//  #include "image.h"

#include <iostream>
using namespace std;

char UTF8[512];
int g_exepathlen = 2;

// #define INDIA

#define _T(en, cn) trantext(en, cn)
#ifndef INDIA
#define GALSDLLNAME "libgals1110.so"
#define GAMCDLLNAME "libgamc1110.so" //
#define SCANNERTYPE "S680"
#define CPRTEN "Copyright (C) Hongda Opto-Electron CO.,LTD."
#define CPRTCH "版权所有(C) 长春鸿达光电子与生物统计识别技术有限公司"
#define TITLEEN "S680 Fingerprint Scanner 2.4"
#define TITLECH "S680 指纹采集仪 2.4"
#define PNGFILE "S68016.png"
#else
#define GALSDLLNAME "libgals0410.so"
#define GAMCDLLNAME "libgamc0410.so" //
#define SCANNERTYPE "LS1000M"
#define TITLEEN "LS1000M Fingerprint Scanner"
#define TITLECH "LS1000M 指纹采集仪"
#define CPRTEN "IBIOS Scanner"
#define CPRTCH "IBIOS 采集仪"
#define PNGFILE "S68016.png"
#endif

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 640
int g_disp_width = (DEFAULT_WIDTH / 2);
int g_disp_height = (DEFAULT_HEIGHT / 2);
int g_PauseCount = 0;

void *g_pdlGals = NULL;
void *g_pdlGamc = NULL;
void *g_phDisp = NULL;
bool g_bInit = 0;
int g_PauseCont = 0;
int g_nRollMode = 4;

//=============================================================================
LIVESCAN_Init_ LS_Init = NULL;
LIVESCAN_Close_ LS_Close = NULL;
LIVESCAN_GetChannelCount_ LS_GetChannelCount = NULL;
LIVESCAN_SetBright_ LS_SetBright = NULL;
LIVESCAN_SetContrast_ LS_SetContrast = NULL;
LIVESCAN_GetBright_ LS_GetBright = NULL;
LIVESCAN_GetContrast_ LS_GetContrast = NULL;
LIVESCAN_GetMaxImageSize_ LS_GetMaxImageSize = NULL;
LIVESCAN_GetCaptWindow_ LS_GetCaptWindow = NULL;
LIVESCAN_SetCaptWindow_ LS_SetCaptWindow = NULL;
LIVESCAN_Setup_ LS_Setup = NULL;
LIVESCAN_GetErrorInfo_ LS_GetErrorInfo = NULL;
LIVESCAN_BeginCapture_ LS_BeginCapture = NULL;
LIVESCAN_GetFPRawData_ LS_GetFPRawData = NULL;
LIVESCAN_EndCapture_ LS_EndCapture = NULL;
LIVESCAN_IsSupportCaptWindow_ LS_IsSupportCaptWindow = NULL;
LIVESCAN_IsSupportSetup_ LS_IsSupportSetup = NULL;
LIVESCAN_GetVersion_ LS_GetVersion = NULL;
LIVESCAN_GetDesc_ LS_GetDesc = NULL;
LIVESCAN_RawToBmp_ LS_RawToBmp = NULL;
LIVESCAN_BmpToRaw_ LS_BmpToRaw = NULL;
LIVESCAN_FeatureExtractMem_ LS_FeatureExtractMem = NULL;
LIVESCAN_FeatureMatchMem_ LS_FeatureMatchMem = NULL;
LIVESCAN_BitmapToWSQ_ LS_BitmapToWSQ = NULL;

MOSAIC_Init_ MC_Init = NULL;
MOSAIC_Close_ MC_Close = NULL;
MOSAIC_IsSupportIdentifyFinger_ MC_IsSupportIdentifyFinger = NULL;
MOSAIC_IsSupportImageQuality_ MC_IsSupportImageQuality = NULL;
MOSAIC_IsSupportFingerQuality_ MC_IsSupportFingerQuality = NULL;
MOSAIC_IsSupportImageEnhance_ MC_IsSupportImageEnhance = NULL;
MOSAIC_IsSupportRollCap_ MC_IsSupportRollCap = NULL;
MOSAIC_SetRollMode_ MC_SetRollMode = NULL;
MOSAIC_Start_ MC_Start = NULL;
MOSAIC_DoMosaic_ MC_DoMosaic = NULL;
MOSAIC_Stop_ MC_Stop = NULL;
MOSAIC_ImageQuality_ MC_ImageQuality = NULL;
MOSAIC_FingerQuality_ MC_FingerQuality = NULL;
MOSAIC_ImageEnhance_ MC_ImageEnhance = NULL;
MOSAIC_IsFinger_ MC_IsFinger = NULL;
MOSAIC_GetErrorInfo_ MC_GetErrorInfo = NULL;
MOSAIC_GetVersion_ MC_GetVersion = NULL;
MOSAIC_GetDesc_ MC_GetDesc = NULL;

//=============================================================================
// pthread_t g_tprcsid = 0;
// pthread_t g_tdspid = 0;

int g_terminate_prcs = 0;
int g_terminate_dsp = 0;

// sem_t g_sem;
bool load_dll();
// void *process_thread(void *param);
// void *display_thread(void *param);
//=============================================================================

// char *gb2utf8(char *putf8, int utf8len, char *gb, int gblen)
// {
//     if (code_convert("GB2312", "utf-8", gb, gblen, putf8, utf8len) > -1)
//     {
//         return putf8;
//     }
//     return NULL;
// }

// char *trantext(char *en, char *cn)
// {
//     return IsChinese() ? cn : en;
// }

int openFpr()
{
    int nRet = LS_Init();
    if (nRet != LS_ERROR_SUCCESS)
    {
        printf(" LIVESCAN_Init failed code[%d]\n", nRet);
    }
    else
    {
        printf(" LIVESCAN_Init succeeded code[%d]\n", nRet);
    }
    return nRet;
}

void closeFpr()
{
    int nRet = LS_Close();
    if (LS_Close() == LS_ERROR_SUCCESS)
    {
        printf(" LIVESCAN_Close succeeded code[%d]\n", nRet);
    }
    else
    {
        printf(" LIVESCAN_Close failed code[%d]\n", nRet);
    }
}

bool load(char *buf, int len)
{
    std::ifstream inp{"old.ori", std::ios_base::binary};
    if (inp && buf != NULL)
    {
        inp.read(buf, len);
        return true;
    }
    return false;
}

void save(const char *buf, int len)
{
    std::ofstream of{"new.ori", std::ios_base::binary};
    if (buf != NULL && of)
    {
        of.write(buf, len);
    }
}

void save(const char *buf, int len, const char *name)
{
    std::ofstream of{name, std::ios_base::binary};
    if (buf != NULL && of)
    {
        of.write(buf, len);
    }
}

// root["position"] = position;
// root["score"] = 99.9;
// root["collectScore"] = 99.9;
// root["feature"] = "手指特诊码";
// root["fingerImage"] = "指纹图像数据流 Base64";
// root["fingerImageCRaw"] = "指纹图片压缩数据 Base64";
// root["sceneVideo"] = "现场视频 Base64";
// root["video"] = "视频(微距) Base64";
// root["realVideo"] = "实时视频 Base64";

int main(int argc, char *argv[])
{
    char xxxx[DEFAULT_WIDTH * DEFAULT_HEIGHT];
    printf("load_dll =%d  sizeof(xxx)=%d\n", load_dll(), sizeof(xxxx));
    int Width = 640;
    int Height = 640;
    int nRet = 0;

    // LS_GetMaxImageSize(0, &Width, &Height);
    BYTE *pTmpBuf = (BYTE *)malloc(Width * Height);
    if (pTmpBuf == NULL)
    {
        printf("Not enough memory\n");
        closeFpr();
        return 1;
    }

    if (!load((char *)pTmpBuf, Width * Height))
    {
        if (openFpr() != LS_ERROR_SUCCESS)
        {
            return 1;
        }

        int nRet = LS_GetChannelCount();
        if (nRet <= 0)
        {
            printf(" LIVESCAN_GetChannelCount failed nRet[%d]\n", nRet);
            closeFpr();
            return nRet;
        }
        printf(" LIVESCAN_GetChannelCount succeeded nRet[%d]\n", nRet);

        nRet = LS_BeginCapture(0);
        if (nRet < 0)
        {
            printf(" LIVESCAN_BeginCapture failed nRet[%d]\n", nRet);
            closeFpr();
            return nRet;
        }
        printf(" LIVESCAN_BeginCapture succeeded nRet[%d]\n", nRet);
        nRet = LS_GetFPRawData(0, (BYTE *)pTmpBuf);
        LS_EndCapture(0);

        save((char *)pTmpBuf, Width * Height);
    }
    // save((char *)pTmpBuf, Width * Height);

    openFpr();
    BYTE *pBmp = (BYTE *)malloc(Width * Height + 1078);
    LS_RawToBmp(pTmpBuf, Width, Height, pBmp, Width * Height + 1078);
    save((const char *)pBmp, Width * Height + 1078, "new.bmp");

    BYTE *pWsq = (BYTE *)malloc(Width * Height + 1078);
    int wLen = Width * Height + 1078;
    nRet = LS_BitmapToWSQ(pBmp, 10, pWsq, &wLen);
    printf("LS_BitmapToWSQ nRet=%d\n", nRet);
    if (nRet == MC_ERROR_SUCCESS)
    {
        save((const char *)pWsq, wLen, "new.wsq");
    }

    unsigned char fea[1024];
    LS_FeatureExtractMem(pTmpBuf, Width * Height, fea);
    printf("fea=[%s]\n", fea);
    closeFpr();
    nRet = MC_Init();
    if (nRet != MC_ERROR_SUCCESS)
    {
        printf("failed to MC_Init code=[%d]\n", nRet);
        return 1;
    }
    int qa = MC_FingerQuality(pTmpBuf, Width, Height);
    printf("qa = %d", qa);
    MC_Close();
    return 0;
}

bool load_dll()
{
    if (g_pdlGals == NULL)
    {
        char *pszErr;
        string filename = string("./mips64/") + GALSDLLNAME;
        if ((g_pdlGals = dlopen(filename.c_str(), RTLD_LAZY)) == NULL) // RTLD_NOW
        {
            filename = string("./amd64/") + GALSDLLNAME;
            g_pdlGals = dlopen(filename.c_str(), RTLD_LAZY);
            if (g_pdlGals == NULL)
            {
                printf("Failed load library: %s\n", GALSDLLNAME);
                printf("Error is: %s\n", dlerror());
                return false;
            }
        }
        if ((pszErr = dlerror()) != NULL)
        {
            printf("dlerror()=%s\n", pszErr);
            g_pdlGals = NULL;
            return false;
        }
        LS_Init = (LIVESCAN_Init_)dlsym(g_pdlGals, "LIVESCAN_Init");
        LS_Close = (LIVESCAN_Close_)dlsym(g_pdlGals, "LIVESCAN_Close");
        LS_GetChannelCount = (LIVESCAN_GetChannelCount_)dlsym(g_pdlGals, "LIVESCAN_GetChannelCount");
        LS_SetContrast = (LIVESCAN_SetContrast_)dlsym(g_pdlGals, "LIVESCAN_SetContrast");
        LS_GetBright = (LIVESCAN_GetBright_)dlsym(g_pdlGals, "LIVESCAN_GetBright");
        LS_SetBright = (LIVESCAN_SetBright_)dlsym(g_pdlGals, "LIVESCAN_SetBright");
        LS_GetContrast = (LIVESCAN_GetContrast_)dlsym(g_pdlGals, "LIVESCAN_GetContrast");
        LS_GetMaxImageSize = (LIVESCAN_GetMaxImageSize_)dlsym(g_pdlGals, "LIVESCAN_GetMaxImageSize");
        LS_GetCaptWindow = (LIVESCAN_GetCaptWindow_)dlsym(g_pdlGals, "LIVESCAN_GetCaptWindow");
        LS_SetCaptWindow = (LIVESCAN_SetCaptWindow_)dlsym(g_pdlGals, "LIVESCAN_SetCaptWindow");
        LS_Setup = (LIVESCAN_Setup_)dlsym(g_pdlGals, "LIVESCAN_Setup");
        LS_GetErrorInfo = (LIVESCAN_GetErrorInfo_)dlsym(g_pdlGals, "LIVESCAN_GetErrorInfo");
        LS_BeginCapture = (LIVESCAN_BeginCapture_)dlsym(g_pdlGals, "LIVESCAN_BeginCapture");
        LS_GetFPRawData = (LIVESCAN_GetFPRawData_)dlsym(g_pdlGals, "LIVESCAN_GetFPRawData");
        LS_EndCapture = (LIVESCAN_EndCapture_)dlsym(g_pdlGals, "LIVESCAN_EndCapture");
        LS_IsSupportCaptWindow = (LIVESCAN_IsSupportCaptWindow_)dlsym(g_pdlGals, "LIVESCAN_IsSupportCaptWindow");
        LS_IsSupportSetup = (LIVESCAN_IsSupportSetup_)dlsym(g_pdlGals, "LIVESCAN_IsSupportSetup");
        LS_GetVersion = (LIVESCAN_GetVersion_)dlsym(g_pdlGals, "LIVESCAN_GetVersion");
        LS_GetDesc = (LIVESCAN_GetDesc_)dlsym(g_pdlGals, "LIVESCAN_GetDesc");
        // LS_SetFrameRate = (LIVESCAN_SetFrameRate_)dlsym(g_pdlGals, "LIVESCAN_SetFrameRate");
        // LS_GetFrameRate = (LIVESCAN_GetFrameRate_)dlsym(g_pdlGals, "LIVESCAN_GetFrameRate");
        LS_RawToBmp = (LIVESCAN_RawToBmp_)dlsym(g_pdlGals, "LIVESCAN_RawToBmp");
        LS_BmpToRaw = (LIVESCAN_BmpToRaw_)dlsym(g_pdlGals, "LIVESCAN_BmpToRaw");
        LS_FeatureExtractMem = (LIVESCAN_FeatureExtractMem_)dlsym(g_pdlGals, "LIVESCAN_FeatureExtractMem");
        LS_FeatureMatchMem = (LIVESCAN_FeatureMatchMem_)dlsym(g_pdlGals, "LIVESCAN_FeatureMatchMem");
        LS_BitmapToWSQ = (LIVESCAN_BitmapToWSQ_)dlsym(g_pdlGals, "LIVESCAN_BitmapToWSQ");

        if ((pszErr = dlerror()) != NULL)
        {
            printf("dlerror()=%s\n", pszErr);
            dlclose(g_pdlGals);
            g_pdlGals = NULL;

            return false;
        }

        filename = string("./mips64/") + GAMCDLLNAME;
        if ((g_pdlGamc = dlopen(filename.c_str(), RTLD_LAZY)) == NULL) // RTLD_NOW
        {
            filename = string("./amd64/") + GAMCDLLNAME;
            g_pdlGamc = dlopen(filename.c_str(), RTLD_LAZY);
            if (g_pdlGamc == NULL)
            {
                printf("Failed load library: %s\n", GAMCDLLNAME);
                printf("Error is: %s\n", dlerror());
                return false;
            }
        }
        if ((pszErr = dlerror()) != NULL)
        {
            printf("dlerror()=%s\n", pszErr);
            g_pdlGamc = NULL;
            return false;
        }
        MC_Init = (MOSAIC_Init_)dlsym(g_pdlGamc, "MOSAIC_Init");
        MC_Close = (MOSAIC_Close_)dlsym(g_pdlGamc, "MOSAIC_Close");
        MC_IsSupportIdentifyFinger = (MOSAIC_IsSupportIdentifyFinger_)dlsym(g_pdlGamc, "MOSAIC_IsSupportIdentifyFinger");
        MC_IsSupportImageQuality = (MOSAIC_IsSupportImageQuality_)dlsym(g_pdlGamc, "MOSAIC_IsSupportImageQuality");
        MC_IsSupportFingerQuality = (MOSAIC_IsSupportFingerQuality_)dlsym(g_pdlGamc, "MOSAIC_IsSupportFingerQuality");
        MC_IsSupportImageEnhance = (MOSAIC_IsSupportImageEnhance_)dlsym(g_pdlGamc, "MOSAIC_IsSupportImageEnhance");
        MC_IsSupportRollCap = (MOSAIC_IsSupportRollCap_)dlsym(g_pdlGamc, "MOSAIC_IsSupportRollCap");
        MC_SetRollMode = (MOSAIC_SetRollMode_)dlsym(g_pdlGamc, "MOSAIC_SetRollMode");
        MC_Start = (MOSAIC_Start_)dlsym(g_pdlGamc, "MOSAIC_Start");
        MC_DoMosaic = (MOSAIC_DoMosaic_)dlsym(g_pdlGamc, "MOSAIC_DoMosaic");
        MC_Stop = (MOSAIC_Stop_)dlsym(g_pdlGamc, "MOSAIC_Stop");
        MC_ImageQuality = (MOSAIC_ImageQuality_)dlsym(g_pdlGamc, "MOSAIC_ImageQuality");
        MC_FingerQuality = (MOSAIC_FingerQuality_)dlsym(g_pdlGamc, "MOSAIC_FingerQuality");
        MC_ImageEnhance = (MOSAIC_ImageEnhance_)dlsym(g_pdlGamc, "MOSAIC_ImageEnhance");
        MC_IsFinger = (MOSAIC_IsFinger_)dlsym(g_pdlGamc, "MOSAIC_IsFinger");
        MC_GetErrorInfo = (MOSAIC_GetErrorInfo_)dlsym(g_pdlGamc, "MOSAIC_GetErrorInfo");
        MC_GetVersion = (MOSAIC_GetVersion_)dlsym(g_pdlGamc, "MOSAIC_GetVersion");
        MC_GetDesc = (MOSAIC_GetDesc_)dlsym(g_pdlGamc, "MOSAIC_GetDesc");
        if ((pszErr = dlerror()) != NULL)
        {
            dlclose(g_pdlGamc);
            g_pdlGamc = NULL;

            return false;
        }
    }

    return g_pdlGals != NULL && g_pdlGamc != NULL;
}
