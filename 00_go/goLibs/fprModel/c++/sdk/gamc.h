#ifndef _GAMC_H_
#define _GAMC_H_

#ifndef WIN32
// #include <X11/Xlib.h>

#ifndef BASETYPES
#define BASETYPES
#define __stdcall
typedef int BOOL;
typedef char TCHAR;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef long LONG;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef void *HWND;

typedef char *PTCHAR;
typedef char *LPSTR;
typedef char *PSTR;
typedef const char *LPCSTR, *PCSTR, *LPCTSTR;
typedef unsigned char *PUCHAR;
typedef void *LPVOID;
typedef DWORD *LPDWORD;
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif
#endif //_WIN32

#define POLICE_USB

#define MC_ERROR_SUCCESS (1)
#define MC_ERROR_INVALID_PARAMETER (-1)
#define MC_ERROR_NOT_ENOUGH_MEMORY (-2)
#define MC_ERROR_NOT_SUPPORT_FUCTION (-3)
#define MC_ERROR_DEVICE_NOT_FOUND (-4)
#define MC_ERROR_DEVICE_NOT_INIT (-5)
#define MC_ERROR_INVALIDE_CODE (-6)
#define MC_ERROR_NO_PRIVILEGE (-7)
#define MC_ERROR_MOSAIC_NOT_INIT (-8)

#define MC_ERROR_UNKNOWN (-101)

#ifdef __cplusplus
extern "C"
{
#endif

	int __stdcall MOSAIC_Init(void);
	int __stdcall MOSAIC_Close(void);
	int __stdcall MOSAIC_IsSupportIdentifyFinger(void);
	int __stdcall MOSAIC_IsSupportImageQuality(void);
	int __stdcall MOSAIC_IsSupportFingerQuality(void);
	int __stdcall MOSAIC_IsSupportImageEnhance(void);
	int __stdcall MOSAIC_IsSupportRollCap(void);
	int __stdcall MOSAIC_SetRollMode(int nRollMode);
	int __stdcall MOSAIC_Start(unsigned char *pFingerBuf, int nWidth, int nHeight);
	int __stdcall MOSAIC_DoMosaic(unsigned char *pDataBuf, int nWidth, int nHeight);
	int __stdcall MOSAIC_Stop(void);
	int __stdcall MOSAIC_ImageQuality(unsigned char *pDataBuf, int nWidth, int nHeight);
	int __stdcall MOSAIC_FingerQuality(unsigned LIVESCAN_Close_char *pDataBuf, int nWidth, int nHeight);
	int __stdcall MOSAIC_ImageEnhance(unsigned char *pSrcBuf, int nWidth, int nHeight, unsigned char *pTargetImg);
	int __stdcall MOSAIC_IsFinger(unsigned char *pDataBuf, int nWidth, int nHeight);
	int __stdcall MOSAIC_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);
	int __stdcall MOSAIC_GetVersion(void);
	int __stdcall MOSAIC_GetDesc(char pszDesc[1024]);

	typedef int(__stdcall *MOSAIC_Init_)(void);
	typedef int(__stdcall *MOSAIC_Close_)(void);
	typedef int(__stdcall *MOSAIC_IsSupportIdentifyFinger_)(void);
	typedef int(__stdcall *MOSAIC_IsSupportImageQuality_)(void);
	typedef int(__stdcall *MOSAIC_IsSupportFingerQuality_)(void);
	typedef int(__stdcall *MOSAIC_IsSupportImageEnhance_)(void);
	typedef int(__stdcall *MOSAIC_IsSupportRollCap_)(void);
	typedef int(__stdcall *MOSAIC_SetRollMode_)(int nRollMode);
	typedef int(__stdcall *MOSAIC_Start_)(unsigned char *pFingerBuf, int nWidth, int nHeight);
	typedef int(__stdcall *MOSAIC_DoMosaic_)(unsigned char *pDataBuf, int nWidth, int nHeight);
	typedef int(__stdcall *MOSAIC_Stop_)(void);
	typedef int(__stdcall *MOSAIC_ImageQuality_)(unsigned char *pDataBuf, int nWidth, int nHeight);
	typedef int(__stdcall *MOSAIC_FingerQuality_)(unsigned char *pDataBuf, int nWidth, int nHeight);
	typedef int(__stdcall *MOSAIC_ImageEnhance_)(unsigned char *pSrcBuf, int nWidth, int nHeight,
												 unsigned char *pTargetImg);
	typedef int(__stdcall *MOSAIC_IsFinger_)(unsigned char *pDataBuf, int nWidth, int nHeight);
	typedef int(__stdcall *MOSAIC_GetErrorInfo_)(int nErrorNo, char pszErrorInfo[256]);
	typedef int(__stdcall *MOSAIC_GetVersion_)(void);
	typedef int(__stdcall *MOSAIC_GetDesc_)(char pszDesc[1024]);

#ifdef __cplusplus
}
#endif

#endif //_GAMC_H_
