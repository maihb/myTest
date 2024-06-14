package main

import (
	"log"
	"runtime"
	"sync"
	"sync/atomic"
	"time"
	"unsafe"
)

/*
#include <stdlib.h>
const char *StrNil = "";
*/
import "C"

const (
	str_rsp  = "STR_RSP"
	err_info = "ERR_INFO"

	less_time_out = 15 //最低缓存超时时间=15 秒
)

var gFpr = CgoData{timeout: less_time_out}

func reco() {
	if err := recover(); err != nil {
		var buf [4096]byte
		n := runtime.Stack(buf[:], false)
		log.Printf("panic %v\n==> %s\n", err, string(buf[:n]))
	}
}

type exData struct {
	last  int64
	value *C.char
}

type CgoData struct {
	keyMap  map[string]*exData
	timeout int64
	once    sync.Once
	mutex   sync.Mutex
}

func (c *CgoData) autoClean() {
	c.once.Do(func() {
		go func() {
			for {
				time.Sleep(30 * time.Second)
				num := 0
				c.mutex.Lock()
				for k, v := range c.keyMap {
					if time.Now().Unix()-atomic.LoadInt64(&v.last) > atomic.LoadInt64(&c.timeout) {
						C.free(unsafe.Pointer(v.value))
						delete(c.keyMap, k)
					}
					num++
					if num > 10 {
						break
					}
				}
				c.mutex.Unlock()
			}
		}()
	})
}

func (c *CgoData) release() {
	c.mutex.Lock()
	defer c.mutex.Unlock()
	for k, v := range c.keyMap {
		C.free(unsafe.Pointer(v.value))
		delete(c.keyMap, k)
	}
	c.keyMap = nil
	//runtime.GC()
}

func (c *CgoData) SetTimeout(t int) {
	if t < less_time_out {
		t = less_time_out
	}
	atomic.StoreInt64(&c.timeout, int64(t))
}

func (c *CgoData) get(key string) *C.char {
	c.mutex.Lock()
	defer c.mutex.Unlock()

	if v, ok := c.keyMap[key]; ok {
		return v.value
	}
	return C.StrNil
}

func (c *CgoData) set(key, value string) *C.char {
	c.mutex.Lock()
	defer c.mutex.Unlock()

	if c.keyMap == nil {
		c.keyMap = make(map[string]*exData)
		c.autoClean()
	}
	cstr := C.CString(value)
	if v, ok := c.keyMap[key]; ok {
		atomic.StoreInt64(&v.last, time.Now().Unix())
		C.free(unsafe.Pointer(v.value))
		v.value = cstr
	} else {
		c.keyMap[key] = &exData{last: time.Now().Unix(), value: cstr}
	}
	return cstr
}

func release() {
	gFpr.release()
}
func getRsp() *C.char {
	return gFpr.get(str_rsp)
}
func setRsp(str string) *C.char {
	return gFpr.set(str_rsp, str)
}

func getErrInfo() *C.char {
	return gFpr.get(err_info)
}
func setErrInfo(str string) {
	gFpr.set(err_info, str)
}
