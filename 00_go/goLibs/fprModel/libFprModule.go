package main

import (
	_ "libFprModule/config"
	"unsafe"

	log "github.com/sirupsen/logrus"
)

/*
typedef const char * Cchar_t;
*/
import "C"

const (
	ERROR_FPR_CLASS         = 12   //指纹采集仪模块
	ERROR_FPR_DEV_NOTEXIST  = 1201 //指纹采集仪设备不存在
	ERROR_FPR_MODULE_UNLOAD = 1202 //指纹采集仪模块加载错误（端口被占用、动态库丢失或加载失败）
	ERROR_FPR_INIT_FAIL     = 1203 //指纹采集仪初始化失败
	ERROR_FPR_NO_INIT       = 1204 //指纹采集仪未初始化
	ERROR_FPR_NO_FUNCTION   = 1205 //指纹采集仪功能未实现
	ERROR_FPR_PARAM_ERR     = 1206 //指纹采集仪参数错误
	ERROR_FPR_MEMORY_ERR    = 1207 //指纹采集仪内存分配失败
	ERROR_FPR_UNAUTHORIZ    = 1208 //指纹采集仪没有授权
	ERROR_FPR_FPRCAP_ERR    = 1296 //指纹采集仪指纹采集失败
	ERROR_FPR_CHECK_ERR     = 1297 //指纹采集仪指纹校验失败
	ERROR_FPR_CMP_ERR       = 1298 //指纹采集仪指纹比对不一致
	ERROR_FPR_UNKNOWN       = 1299 //指纹采集仪未知错误
)

//export fprModuleInit
func fprModuleInit(path C.Cchar_t, logLevel C.int) C.int {
	log.Infof("fprModuleInit successful")
	return 0
}

//export fprModuleRelease
func fprModuleRelease() {
	log.Println("fprModuleRelease")
	release()
}

//export fprReadInfo
func fprReadInfo(timeout C.int) C.int {
	log.Println(timeout)
	log.Println("fprReadInfo")
	return 1
}

//export fprGetInfo
func fprGetInfo(useType, leftRight, position C.int, historyFeature *C.char, errCode *C.int) C.Cchar_t {
	rsp, err := getInfo(int(useType), int(leftRight), int(position), C.GoString(historyFeature))
	if errCode != nil {
		code := (*int)(unsafe.Pointer(errCode))
		if err != nil {
			*code = ERROR_FPR_MODULE_UNLOAD
			return setRsp(err.Error())
		}
		*code = 0
	}
	return setRsp(rsp)
}

//export fprGetLastErrorInfo
func fprGetLastErrorInfo() C.Cchar_t {
	setErrInfo("hello fprGetLastErrorInfo")
	return getErrInfo()
}

//export fprReadStop
func fprReadStop() {
	log.Println("fprReadStop")
}

func main() {}
