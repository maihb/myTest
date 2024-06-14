package clibs

import (
	"fmt"
	"unsafe"
)

/*
#cgo CFLAGS: -I./cf900
#cgo amd64 LDFLAGS: ./lib/fpr_amd64/libgamc1110.so
#cgo arm64 LDFLAGS: ./lib/fpr_arm64/libgamc1110.so
#include "gamc.h"
*/
import "C"

func McGetErrorInfo(code C.int) error {
	if code == C.MC_ERROR_SUCCESS {
		return nil
	}
	var data [256]byte
	p := (*C.char)(unsafe.Pointer(&data[0]))
	C.MOSAIC_GetErrorInfo(code, p)
	err := fmt.Errorf("%s", data)
	return err
}

func McInit() error {
	return McGetErrorInfo(C.MOSAIC_Init())
}
func McClose() error {
	return McGetErrorInfo(C.MOSAIC_Close())
}

func McGetVersion() int {
	return int(C.MOSAIC_GetVersion())
}

// int __stdcall MOSAIC_FingerQuality(unsigned char *pDataBuf, int nWidth, int nHeight);
func McFingerQuality(data []byte, width, height int) (int, error) {
	p := (*C.uchar)(unsafe.Pointer(&data[0]))
	q := C.MOSAIC_FingerQuality(p, C.int(width), C.int(height))
	iq := int(q)
	if iq < 0 {
		return iq, McGetErrorInfo(q)
	}
	return iq, nil
}
