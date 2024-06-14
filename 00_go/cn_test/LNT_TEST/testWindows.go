//go:build windows
// +build windows

package main

import (
	"fmt"
	"log"
	"os"
	"runtime"
	"syscall"
)

func init() {
	test = testWin
}

// int  LNTReaderInit(const char* pszAppPath, const char *pszModulePath);
// void  LNTReaderRelease(void);
// int  LNTReaderReadCardInit(int timeout);
// void  LNTReaderStopRead();
// const char * LNTReaderGetCardInitInfo(void);
// int  LNTReaderSendAPDU(const char *pszData);
// const char * LNTReaderGetAPDUResultInfo(void);
// int  LNTReaderReadCardEnd();
// const char * LNTReaderGetLastErrorInfo(void);

type dllFile struct {
	LNTReaderInit              *syscall.LazyProc
	LNTReaderRelease           *syscall.LazyProc
	LNTReaderReadCardInit      *syscall.LazyProc
	LNTReaderStopRead          *syscall.LazyProc
	LNTReaderGetCardInitInfo   *syscall.LazyProc
	LNTReaderSendAPDU          *syscall.LazyProc
	LNTReaderGetAPDUResultInfo *syscall.LazyProc
	LNTReaderReadCardEnd       *syscall.LazyProc
	LNTReaderGetLastErrorInfo  *syscall.LazyProc
}

func loadDL(dl_name string) (d *dllFile, err error) {
	defer func() {
		if errReco := recover(); errReco != nil {
			err = fmt.Errorf("%v", errReco)
			var buf [4096]byte
			n := runtime.Stack(buf[:], false)
			log.Printf("panic %v\n==> %s\n", err, string(buf[:n]))
		}
	}()
	base := syscall.NewLazyDLL(dl_name)
	d = &dllFile{
		//libc:                  base,
		LNTReaderInit:              base.NewProc("LNTReaderInit"),
		LNTReaderRelease:           base.NewProc("LNTReaderRelease"),
		LNTReaderReadCardInit:      base.NewProc("LNTReaderReadCardInit"),
		LNTReaderStopRead:          base.NewProc("LNTReaderStopRead"),
		LNTReaderGetCardInitInfo:   base.NewProc("LNTReaderGetCardInitInfo"),
		LNTReaderSendAPDU:          base.NewProc("LNTReaderSendAPDU"),
		LNTReaderGetAPDUResultInfo: base.NewProc("LNTReaderGetAPDUResultInfo"),
		LNTReaderReadCardEnd:       base.NewProc("LNTReaderReadCardEnd"),
		LNTReaderGetLastErrorInfo:  base.NewProc("LNTReaderGetLastErrorInfo"),
	}
	return
}

func testWin() {
	dll_name := "libLNTModule_GRG.dll"
	if len(os.Args) > 1 {
		dll_name = os.Args[1]
	}
	log.Println("try to test dll_name=", dll_name)
	d, err := loadDL(dll_name)
	if err != nil {
		log.Println("loadDL failed, err=", err)
		return
	}
	cwd, err := os.Getwd()
	if err != nil {
		fmt.Println("Error:", err)
		return
	}

	r, _, _ := d.LNTReaderInit.Call(WinStr8(cwd), WinStr8(cwd+"\\driver\\IdcardModule\\GRG_CRT"))
	if int16(r) != 0 {
		re, _, _ := d.LNTReaderGetLastErrorInfo.Call()
		log.Printf("LNTReaderInit failed, r=%d, lastErr=%s", int16(r), GoString(re))
		return
	}
	defer d.LNTReaderRelease.Call()

	r, _, _ = d.LNTReaderReadCardInit.Call(uintptr(10))
	if int32(r) != 0 {
		re, _, _ := d.LNTReaderGetLastErrorInfo.Call()
		log.Printf("LNTReaderReadCardInit failed, r=%d, lastErr=%s", int32(r), GoString(re))
		return
	}
	defer d.LNTReaderReadCardEnd.Call()

	r, _, _ = d.LNTReaderGetCardInitInfo.Call()
	log.Printf("LNTReaderGetCardInitInfo return=%s", GoString(r))

	log.Println("done")
}
