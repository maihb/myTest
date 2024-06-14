package main

import (
	"log"
)

/*
typedef const char * Cchar_t;
*/
import "C"

var sNum int = 0

//export getxxxx
func getxxxx(useType, leftRight, position C.int, historyFeature *C.char, errCode *C.int) C.Cchar_t {
	return gFpr.get(C.GoString(historyFeature))
}

//export getInfo
func getInfo(key *C.char) C.Cchar_t {
	defer reco()
	return gFpr.get(C.GoString(key))
}

//export setInfo
func setInfo(ck, cv C.Cchar_t) C.int {
	defer reco()
	k, v := C.GoString(ck), C.GoString(cv)
	sNum++
	log.Printf("setInfo: k[%s],v[%s], sNum[%d] \n", k, v, sNum)
	gFpr.set(k, v)
	return 0
}
