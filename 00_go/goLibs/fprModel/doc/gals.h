#ifndef _GALS_H_
#define _GALS_H_

#ifndef BASETYPES
#define BASETYPES
#ifndef WIN32
    #define __stdcall
#endif
typedef int                 BOOL;
typedef char TCHAR;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef long				LONG;
typedef unsigned long       DWORD;
typedef unsigned long 		ULONG;
typedef unsigned int        UINT;
//typedef void *				HWND;

typedef char *PTCHAR;
typedef char *LPSTR;
typedef char *PSTR;
typedef const char *LPCSTR, *PCSTR, *LPCTSTR;
typedef unsigned char 	*PUCHAR;
typedef void             *LPVOID;
typedef DWORD            *LPDWORD;
#endif
#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define  FP_IMAGE_BMP                       1


#define LS_ERROR_SUCCESS                   (1)
#define LS_ERROR_INVALID_PARAMETER         (-1)
#define LS_ERROR_NOT_ENOUGH_MEMORY         (-2)
#define LS_ERROR_NOT_SUPPORT_FUNCTION      (-3)
#define LS_ERROR_DEVICE_NOT_FOUND          (-4)
#define LS_ERROR_DEVICE_NOT_INIT           (-5)
#define LS_ERROR_INVALIDE_CODE             (-6)
#define LS_ERROR_NO_PRIVILEGE              (-7)

#define LS_ERROR_UNKNOWN                   (-101)
#define LS_ERROR_ACCESS_DEVICE             (-102)
#define LS_ERROR_NO_ENOUPH_BANDWIDTH       (-103)
#define LS_ERROR_VIDEO_NOT_INIT            (-104)
#define LS_ERROR_ACCESS_FILE               (-105)
#define LS_ERROR_DLL_NOT_FOUND             (-106)
#define LS_ERROR_TIMEOUT                   (-107)
#define LS_ERROR_ALREADYCANCLE             (-108)
#define LS_ERROR_DATA_LOST                 (-109)
#define LS_ERROR_PREVIEW_MODE              (-110)
#define LS_ERROR_NO_SERIAL_IN_DEVICE       (-111)


#pragma pack(push,1)
#ifndef FP_PRODUCT_INFO
#define FP_PRODUCT_INFO
typedef struct _Fp_Product_Info //28?ֽ?
{
	char Serial[20];
	struct  
	{
		WORD Year;
		char Month;
		char Day;
	}Date;
	char Operater[2];
    char Reserved[2];
}Fp_Product_Info;
typedef struct _Fp_Image_Attr
{
	char bAutoAdjust; //是否启用自动调节
	char bClearBk;	  //是否清除背景残留
	char bAutoPos;    //采集小图时候是否自动定位指纹位置

	BYTE Bright;	 //亮度
	BYTE Contrast;	 //对比度

}Fp_Image_Attr;
#endif
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif
#ifdef WIN32
        typedef int (__stdcall *LIVESCAN_BEGINVIDEO_)(HDC hdc, struct RECT *prct, BYTE *pBuf, int *pnWidth, int *pnHeight);
        typedef int (__stdcall *LIVESCAN_ENDVIDEO_)(HDC hdc, struct RECT *prct, BYTE *pBuf, int nWidth, int nHeight);
#endif
    int __stdcall LIVESCAN_Init(void);//ID
    int __stdcall LIVESCAN_Close(void);//ID
    int __stdcall LIVESCAN_GetChannelCount(void);//ID
    int __stdcall LIVESCAN_SetBright(int nChannel, int nBright);
    int __stdcall LIVESCAN_SetContrast(int nChannel, int nContrast);
    int __stdcall LIVESCAN_GetBright(int nChannel, int *pnBright);
    int __stdcall LIVESCAN_GetContrast(int nChannel, int *pnContrast);
    int __stdcall LIVESCAN_GetMaxImageSize(int nChannel, int *pnWidth, int *pnHeight);
    int __stdcall LIVESCAN_GetCaptWindow(int nChannel, int *pnOriginX, int *pnOriginY, int *pnWidth, int *pnHeight);
    int __stdcall LIVESCAN_SetCaptWindow(int nChannel, int nOriginX, int nOriginY,int nWidth, int nHeight);
    int __stdcall LIVESCAN_Setup(void);
    int __stdcall LIVESCAN_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);//ID
    int __stdcall LIVESCAN_BeginCapture(int nChannel);
    int __stdcall LIVESCAN_GetFPRawData(int nChannel, unsigned char *pRawData);
    int __stdcall LIVESCAN_EndCapture(int nChannel);
    int __stdcall LIVESCAN_IsSupportCaptWindow(int nChannel);
    int __stdcall LIVESCAN_IsSupportSetup(void);
	int __stdcall LIVESCAN_GetPreviewImageSize(int nChannel, int *pnWidth,int *pnHeight);
	int __stdcall LIVESCAN_GetPreviewData(int nChannel, unsigned char *pRawData);
	int __stdcall LIVESCAN_IsSupportPreview (void);
	int __stdcall LIVESCAN_GetVersion(void);
    int __stdcall LIVESCAN_GetDesc(char pszDesc[1024]);//ID


	//Expand functions
	
	int __stdcall LIVESCAN_Get_Version(char *pcVersionInfo);
	int __stdcall LIVESCAN_FeatureExtractMem(unsigned char *  pcImageData, int lDataLen, unsigned char *pcTemplateData);
	int __stdcall LIVESCAN_FeatureMatchMem (unsigned char * pcTemplateDataA, unsigned char *pcTemplateDataB, 	float *pfSim);
	int __stdcall LIVESCAN_GetFPBmpData(int nChannel, unsigned char *pBmpData);//ID
	
#ifdef WIN32
	int __stdcall LIVESCAN_SetVideoWindow (HWND hWnd);
	int __stdcall LIVESCAN_SetVideoWindowEx(HWND hWnd, int nLeft, int nTop, int nWidth, int nHeight);
	int __stdcall LIVESCAN_SetBeginVideo(LIVESCAN_BEGINVIDEO_ pfnBeginVideo);
	int __stdcall LIVESCAN_SetEndVideo(LIVESCAN_ENDVIDEO_ pfnEndVideo);
	#ifndef Q_OS_WIN
	int __stdcall LIVESCAN_ChangePalette(int nStart, int nLen, LPPALETTEENTRY lppe);
	#endif
#endif
	int __stdcall LIVESCAN_SaveToFile(BYTE *pBuf, int nWidth, int nHeight,	LPCSTR szFile, int nFormat);
	//int __stdcall LIVESCAN_AutoAdjust(int nChannel, DWORD dwState);//δʵ??//dwState: 0 - Stop, 1-check, 0xFFFFFFFF-Use Adjust
	int __stdcall LIVESCAN_GetProductInfo(int nChannel, Fp_Product_Info *pi);
	int __stdcall LIVESCAN_SetImgAttr(int nChannel, Fp_Image_Attr dia, BOOL bSave);
	int __stdcall LIVESCAN_GetImgAttr(int nChannel, Fp_Image_Attr *pdia, BOOL bSave);
	//int __stdcall LIVESCAN_SetFrameRate(int nChannel, int nRate); //δʵ??
	//int __stdcall LIVESCAN_GetFrameRate(int nChannel, int *pnRate); //δʵ??
	int __stdcall LIVESCAN_GetFPBmpDataEx(int nChannel, unsigned char *pBmpData, int nWidth, int nHeight);
	int __stdcall LIVESCAN_GetDevCount(void);
	int __stdcall LIVESCAN_SetActiveDev(int nIndex);
	int __stdcall LIVESCAN_InitEx(int nDevIndex);
	//int __stdcall LIVESCAN_FormatRawData(unsigned char *pRaw, int nWidth, int nHeight, unsigned char *pImageData, int nBufLen, int nFormat);
	int __stdcall LIVESCAN_BitmapToWSQ(BYTE *pBmpData, int nCompressRatio, BYTE *pWsq,  int *pnLen);
	int __stdcall LIVESCAN_WSQToBitmap(BYTE *pWsq,  int nSrcLen, BYTE *pBmpData,  int *pnLen);
	int __stdcall LIVESCAN_RawToBmp(unsigned char *pRaw, int nWidth, int nHeight, unsigned char *pBmpBuf, int nBufLen);
	int __stdcall LIVESCAN_BmpToRaw(unsigned char *pBmpBuf, int nBufLen, unsigned char *pRaw, int nRawLen, int *pnWidth, int *pnHeight);
	int __stdcall LIVESCAN_GetUserSpace(int nChannel);
	int __stdcall LIVESCAN_WriteUserData(int nChannel, LPVOID pBuf, DWORD dwAddr, DWORD dwLen);
	int __stdcall LIVESCAN_ReadUserData(int nChannel, LPVOID pBuf, DWORD dwAddr, DWORD dwLen);


    //define functions -----------------------------------------------------------------------------------------------------------
	typedef int (__stdcall *LIVESCAN_Init_)(void);
    typedef int (__stdcall *LIVESCAN_Close_)(void);
    typedef int (__stdcall *LIVESCAN_GetChannelCount_)(void);
    typedef int (__stdcall *LIVESCAN_SetBright_)(int nChannel, int nBright);
    typedef int (__stdcall *LIVESCAN_SetContrast_)(int nChannel, int nContrast);
    typedef int (__stdcall *LIVESCAN_GetBright_)(int nChannel, int *pnBright);
    typedef int (__stdcall *LIVESCAN_GetContrast_)(int nChannel, int *pnContrast);
    typedef int (__stdcall *LIVESCAN_GetMaxImageSize_)(int nChannel, int *pnWidth, int *pnHeight);
    typedef int (__stdcall *LIVESCAN_GetCaptWindow_)(int nChannel, int *pnOriginX, int *pnOriginY, int *pnWidth, int *pnHeight);
    typedef int (__stdcall *LIVESCAN_SetCaptWindow_)(int nChannel, int nOriginX, int nOriginY, int nWidth, int nHeight);
    typedef int (__stdcall *LIVESCAN_Setup_)(void);
    typedef int (__stdcall *LIVESCAN_GetErrorInfo_)(int nErrorNo, char pszErrorInfo[256]);
    typedef int (__stdcall *LIVESCAN_BeginCapture_)(int nChannel);
    typedef int (__stdcall *LIVESCAN_GetFPRawData_)(int nChannel, unsigned char *pRawData);
    typedef int (__stdcall *LIVESCAN_EndCapture_)(int nChannel);
    typedef int (__stdcall *LIVESCAN_IsSupportCaptWindow_)(int nChannel);
    typedef int (__stdcall *LIVESCAN_IsSupportSetup_)(void);
	typedef int (__stdcall *LIVESCAN_GetPreviewImageSize_)(int nChannel, int *pnWidth,int *pnHeight);
	typedef int (__stdcall *LIVESCAN_GetPreviewData_)(int nChannel, unsigned char *pRawData);
	typedef int (__stdcall *LIVESCAN_IsSupportPreview_)(void);
	typedef int (__stdcall *LIVESCAN_GetPreviewImageSize_)(int nChannel, int *pnWidth,int *pnHeight);
	typedef int (__stdcall *LIVESCAN_GetPreviewData_)(int nChannel, unsigned char *pRawData);
	typedef int (__stdcall *LIVESCAN_IsSupportPreview_)(void);
    typedef int (__stdcall *LIVESCAN_GetVersion_)(void);
    typedef int (__stdcall *LIVESCAN_GetDesc_)(char pszDesc[1024]);


	//Expand functions
#ifdef WIN32
	typedef int (__stdcall *LIVESCAN_SetVideoWindow_)(HWND hWnd);
	typedef int (__stdcall *LIVESCAN_SetVideoWindowEx_)(HWND hWnd, int nLeft, int nTop, int nWidth, int nHeight);
	typedef int (__stdcall *LIVESCAN_SetBeginVideo_)(LIVESCAN_BEGINVIDEO_ pfnBeginVideo);
	typedef int (__stdcall *LIVESCAN_SetEndVideo_)(LIVESCAN_ENDVIDEO_ pfnEndVideo);
	#ifndef Q_OS_WIN
	typedef int (__stdcall *LIVESCAN_ChangePalette_)(int nStart, int nLen, LPPALETTEENTRY lppe);
	#endif
#endif

	typedef int (__stdcall *LIVESCAN_SaveToFile_)(unsigned char *pBuf, int nWidth, int nHeight,	LPCSTR szFile, int nFormat);
//	typedef int (__stdcall *LIVESCAN_AutoAdjust_)(int nChannel, DWORD dwState);
	typedef int (__stdcall *LIVESCAN_GetProductInfo_)(int nChannel, Fp_Product_Info *pi);
	typedef int(__stdcall *LIVESCAN_SetImgAttr_)(int nChannel, Fp_Image_Attr dia, BOOL bSave);
	typedef int(__stdcall *LIVESCAN_GetImgAttr_)(int nChannel, Fp_Image_Attr *pdia, BOOL bSave);
	//typedef int (__stdcall *LIVESCAN_SetFrameRate_)(int nChannel, int nRate);
	//typedef int (__stdcall *LIVESCAN_GetFrameRate_)(int nChannel, int *pnRate);
	typedef int (__stdcall *LIVESCAN_Get_Version_)(char *pcVersionInfo);
	typedef int (__stdcall *LIVESCAN_GetFPBmpData_)(int nChannel, unsigned char *pBmpData);
	typedef int (__stdcall *LIVESCAN_GetFPBmpDataEx_)(int nChannel, unsigned char *pBmpData, int nWidth, int nHeight);
	typedef int (__stdcall *LIVESCAN_FeatureExtractMem_)(unsigned char *  pcImageData, int lDataLen, unsigned char *pcTemplateData);
	typedef int (__stdcall *LIVESCAN_FeatureMatchMem_)(unsigned char * pcTemplateDataA, unsigned char *pcTemplateDataB, float *pfSim);
	typedef int (__stdcall *LIVESCAN_GetDevCount_)(void);
	typedef int (__stdcall *LIVESCAN_SetActiveDev_)(int nIndex);
	typedef int (__stdcall *LIVESCAN_InitEx_)(int nDevIndex);
	typedef int (__stdcall *LIVESCAN_FormatRawData_)(unsigned char *pRaw, int nWidth, int nHeight, unsigned char *pImageData, int nBufLen, int nFormat);
	typedef int (__stdcall *LIVESCAN_BitmapToWSQ_)(BYTE *pBmpData, int nCompressRatio, BYTE *pWsq,  int *pnLen);
	typedef int (__stdcall *LIVESCAN_WSQToBitmap_)(BYTE *pWsq,  int nSrcLen, BYTE *pBmpData,  int *pnLen);
	typedef int (__stdcall *LIVESCAN_RawToBmp_)(unsigned char *pRaw, int nWidth, int nHeight, unsigned char *pBmpBuf, int nBufLen);
	typedef int (__stdcall *LIVESCAN_BmpToRaw_)(unsigned char *pBmpBuf, int nBufLen, unsigned char *pRaw, int nRawLen, int *pnWidth, int *pnHeight);
	typedef int (__stdcall *LIVESCAN_GetUserSpace_)(int nChannel);
	typedef int (__stdcall *LIVESCAN_WriteUserData_)(int nChannel, LPVOID pBuf, DWORD dwAddr, DWORD dwLen);
	typedef int (__stdcall *LIVESCAN_ReadUserData_)(int nChannel, LPVOID pBuf, DWORD dwAddr, DWORD dwLen);


#ifdef __cplusplus
}
#endif


#endif //_GALS_H_