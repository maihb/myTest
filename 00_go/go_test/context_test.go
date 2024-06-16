package main

/*如果struct{}的实例逃逸到heap上，那它们的地址可能相同。事实上，The Go Programming Language Specification: Size and alignment guarantees确认了：
 A struct or array type has size zero if it contains no fields (or elements, respectively) that have a size greater than zero. Two distinct zero-size variables may have the same address in memory.
 struct或者array如果其不包含size大于0的字段，则其size为0。两个不同的size为0的变量可能拥有同一个地址。
因此，emptyCtx类型不使用struct{}，为了确保todo和background拥有不同的地址。
*/

import (
	"context"
	"fmt"
	"runtime"
	"testing"
	"time"
)

type S struct{}

func s1() {
	s1, s2 := S{}, S{}
	println(&s1 == &s2)
}

func s2() {
	s1, s2 := S{}, S{}
	println(&s1 == &s2)
	fmt.Printf("%p, %p, %t\n", &s1, &s2, &s1 == &s2)
}

func TestStruct(t *testing.T) {
	s1()
	s2()
}

type CustomContext struct {
	context.Context
	c chan struct{}
}

func (c *CustomContext) Done() <-chan struct{} {
	return c.c
}

func TestCancel33(t *testing.T) {
	a := "abc"
	for _, v := range a {
		fmt.Println(v)
	}
}

func TestCancel(t *testing.T) {
	println(runtime.NumGoroutine()) // 2
	inner, cancel := context.WithCancel(context.Background())
	defer cancel()
	println(runtime.NumGoroutine()) // 2
	c := &CustomContext{inner, make(chan struct{})}
	_, cancel2 := context.WithCancel(c)
	defer cancel2()
	println(runtime.NumGoroutine()) // 3
}

func TestCn(tdd *testing.T) {
	stop := make(chan struct{})
	go func() {
		for {
			select {
			case v := <-stop: // 收到了停滞信号
				fmt.Println(v, "监控1退出，停止了...")
				return
			default:
				fmt.Println("goroutine监控中...1111")
				time.Sleep(2 * time.Second)
			}
		}
	}()

	go func() {
		for {
			select {
			case v := <-stop: // 收到了停滞信号
				fmt.Println(v, "监控2退出，停止了...")
				return
			default:
				fmt.Println("goroutine监控中...222")
				time.Sleep(2 * time.Second)
			}
		}
	}()
	time.Sleep(5 * time.Second)
	fmt.Println("可以了，通知监控停止")
	//stop <- true
	stop <- struct{}{}
	//close(stop)
	//为了检测监控过是否停止，如果没有监控输出，就表示停止了
	time.Sleep(3 * time.Second)
}

func TestSdf(aaa *testing.T) {
	ctx, cancel := context.WithCancel(context.Background())
	go func(ctx context.Context) {
		for {
			select {
			case <-ctx.Done():
				fmt.Println("监控退出，停止了...")
				return
			default:
				fmt.Println("goroutine监控中...")
				time.Sleep(2 * time.Second)
			}
		}
	}(ctx)

	time.Sleep(10 * time.Second)
	fmt.Println("可以了，通知监控停止")
	cancel()
	//为了检测监控过是否停止，如果没有监控输出，就表示停止了
	time.Sleep(5 * time.Second)
}
