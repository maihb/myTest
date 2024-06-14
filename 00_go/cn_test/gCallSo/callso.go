package main

/*
#cgo amd64 LDFLAGS: ./lib/fpr_amd64/libgals1110.so
#cgo arm64 LDFLAGS: "./../cSo/C.so"
#include <stdlib.h>
#include "../cSo/my.h"
*/
import "C"
import (
	"log"
	"runtime"
)

func t1() {
	defer reco()
	log.Println("bbbbbbbbbbbbbb")
	C.say_hello()
	log.Println("      end !!!")
}

func reco() {
	if err := recover(); err != nil {
		var buf [4096]byte
		n := runtime.Stack(buf[:], false)
		log.Printf("panic %v\n==> %s\n", err, string(buf[:n]))
	}
}
