package main

import (
	"log"
	"os"
)

var test func()

func init() {
	//golib.LogInit("log/faceCam.log")
}

func main() {
	log.SetOutput(os.Stdout)
	test()
}
