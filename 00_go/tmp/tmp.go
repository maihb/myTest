package main

import (
	"fmt"
	"log"
)

var a = 9
var buf = make([]byte, 1024)
var cache []byte

func main() {

	for _, v := range pack(0x34, 0x33) {
		fmt.Printf(" 0x%02X,", v)
	}

}

func pack(deng, status byte) []byte {
	//02 4F 06 32 33 4A
	data := []byte{0x02, 0x4F, 0x06}
	data = append(data, deng, status)
	var crc byte
	for i := 0; i < len(data); i++ {
		crc ^= data[i]
	}
	return append(data, crc)
}

func packCmd(cmd string) []byte {
	data := []byte{0x02}
	l := int16(len(cmd))
	bb := []byte(cmd)
	data = append(data, byte(l))    //低 8 位
	data = append(data, byte(l>>8)) //高 8 位
	data = append(data, bb...)
	var sum int16
	for _, v := range bb {
		sum += int16(v)
	}
	data = append(data, byte(sum))    //低 8 位
	data = append(data, byte(sum>>8)) //高 8 位
	data = append(data, 0x03)
	return data
}
func unPackCmd(mi []byte) string {
	if len(mi) < 7 {
		log.Println("unPackCmd failed,len(mi)<7")
		return ""
	}
	if mi[0] == 0x05 {
		mi = mi[1:]
	}
	miLen := len(mi)
	if mi[0] != 0x02 || mi[len(mi)-1] != 0x03 {
		log.Printf("unPackCmd failed, mi=[%02x]", mi)
		return ""
	}
	var l int16 = int16(mi[1]) + int16(mi[2])<<8
	if l != int16(miLen-2-4) {
		log.Println("unPackCmd failed, lenSum not right")
		return ""
	}
	return string(mi[3 : miLen-4])
}
