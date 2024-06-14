package clibs

import (
	"fmt"
	"unsafe"
)

/*
#cgo CFLAGS: -I./cf900
#cgo amd64 LDFLAGS: ./lib/fpr_amd64/libgals1110.so
#cgo arm64 LDFLAGS: ./lib/fpr_arm64/libgals1110.so
#include "gals.h"
#include <stdlib.h>
*/
import "C"

const (
	bmp_header_len = 1078
)

func lsGetErrorInfo(code C.int) error {
	if code >= C.LS_ERROR_SUCCESS {
		return nil
	}
	var data [256]byte
	p := (*C.char)(unsafe.Pointer(&data[0]))
	C.LIVESCAN_GetErrorInfo(code, p)
	err := fmt.Errorf("%s", data)
	return err
}

func LsInit() error {
	return lsGetErrorInfo(C.LIVESCAN_Init())
}
func LsClose() error {
	return lsGetErrorInfo(C.LIVESCAN_Close())
}
func LsGetVersion() int {
	return int(C.LIVESCAN_GetVersion())
}
func LsGetChannelCount() (int, error) {
	num := C.LIVESCAN_GetChannelCount()
	gNum := int(num)
	if gNum < 0 {
		return gNum, lsGetErrorInfo(num)
	}
	return gNum, nil
}
func LsBeginCapture(channel int) error {
	return lsGetErrorInfo(C.LIVESCAN_BeginCapture(C.int(channel)))
}

// int __stdcall LIVESCAN_EndCapture(int nChannel);
func LsEndCapture(channel int) error {
	return lsGetErrorInfo(C.LIVESCAN_EndCapture(C.int(channel)))
}

// int __stdcall LIVESCAN_GetFPRawData(int nChannel, unsigned char *pRawData);
func LsGetFPRawData(channel, width, height int) ([]byte, error) {
	data := make([]byte, width*height)
	p := (*C.uchar)(unsafe.Pointer(&data[0]))
	code := C.LIVESCAN_GetFPRawData(C.int(channel), p)
	return data, lsGetErrorInfo(code)
}

// int __stdcall LIVESCAN_RawToBmp(unsigned char *pRaw, int nWidth, int nHeight, unsigned char *pBmpBuf, int nBufLen);
func LsRawToBmp(data []byte, width, height int) ([]byte, error) {
	pIn := (*C.uchar)(unsafe.Pointer(&data[0]))
	l := width*height + bmp_header_len
	retData := make([]byte, l)
	pOut := (*C.uchar)(unsafe.Pointer(&retData[0]))
	code := C.LIVESCAN_RawToBmp(pIn, C.int(width), C.int(height), pOut, C.int(l))
	if err := lsGetErrorInfo(code); err != nil {
		return nil, err
	}
	return retData, nil
}

// int __stdcall LIVESCAN_BitmapToWSQ(BYTE *pBmpData, int nCompressRatio, BYTE *pWsq,  int *pnLen);
func LsBitmapToWSQ(data []byte, nCompressRatio int) ([]byte, error) {
	pIn := (*C.uchar)(unsafe.Pointer(&data[0]))
	l := len(data)
	retData := make([]byte, l)
	pl := (*C.int)(unsafe.Pointer(&l))
	pOut := (*C.uchar)(unsafe.Pointer(&retData[0]))
	code := C.LIVESCAN_BitmapToWSQ(pIn, C.int(nCompressRatio), pOut, pl)
	if err := lsGetErrorInfo(code); err != nil {
		return nil, err
	}
	if l > len(retData) {
		return nil, fmt.Errorf("LIVESCAN_BitmapToWSQ failed: pnLen[%d], len(retData)=%d", l, len(retData))
	}
	return retData[:l], nil
}

// int __stdcall LIVESCAN_FeatureExtractMem(unsigned char *  pcImageData, int lDataLen, unsigned char *pcTemplateData);
func LsFeatureExtractMem(data []byte) ([]byte, int, error) {
	pIn := (*C.uchar)(unsafe.Pointer(&data[0]))
	l := len(data)
	retData := make([]byte, 1024)
	pOut := (*C.uchar)(unsafe.Pointer(&retData[0]))
	qa := C.LIVESCAN_FeatureExtractMem(pIn, C.int(l), pOut)
	return retData, int(qa), nil
}
