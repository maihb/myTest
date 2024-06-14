package main

import "unsafe"

func GoString(r uintptr) string {
	p := (*byte)(unsafe.Pointer(r))
	data := make([]byte, 0)
	for *p != 0 {
		data = append(data, *p)
		r += unsafe.Sizeof(byte(0))    // 移动指针，指向下一个char
		p = (*byte)(unsafe.Pointer(r)) // 获取指针的值，此时指针已经指向下一个char
	}
	return string(data)
}
func WinStr8(str string) uintptr {
	b := []byte(str)
	b = append(b, 0x00)
	return uintptr(unsafe.Pointer(&b[0]))
}
