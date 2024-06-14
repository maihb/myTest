package main

import (
	"fmt"
	"os"
	"runtime"
	"runtime/trace"
	"time"
)

func abc() {
	// 执行大量的计算操作
	start := time.Now()
	for time.Since(start) < 2*time.Second {
		// 模拟计算密集型工作
		for i := 0; i < 1000000; i++ {
			_ = i * i
		}
	}
}

func main() {
	f, err := os.Create("trace.out")
	if err != nil {
		panic(err)
	}
	defer f.Close()

	currentGOMAXPROCS := runtime.GOMAXPROCS(0)
	fmt.Println("Current GOMAXPROCS:", currentGOMAXPROCS)

	err = trace.Start(f)
	if err != nil {
		panic(err)
	}
	defer trace.Stop()
	for i := 0; i < 10; i++ {
		go abc()
	}
	time.Sleep(time.Second * 4)
	fmt.Println("hello")
}
