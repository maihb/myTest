package main

/*如果struct{}的实例逃逸到heap上，那它们的地址可能相同。事实上，The Go Programming Language Specification: Size and alignment guarantees确认了：
 A struct or array type has size zero if it contains no fields (or elements, respectively) that have a size greater than zero. Two distinct zero-size variables may have the same address in memory.
 struct或者array如果其不包含size大于0的字段，则其size为0。两个不同的size为0的变量可能拥有同一个地址。
因此，emptyCtx类型不使用struct{}，为了确保todo和background拥有不同的地址。
*/

import (
	"fmt"
	"testing"
	"time"

	"github.com/hashicorp/mdns"
)

func TestMdns(t *testing.T) {
	// Make a channel for results and start listening
	entriesCh := make(chan *mdns.ServiceEntry, 10)
	go func() {
		for entry := range entriesCh {
			fmt.Printf("Got new entry: [%v] \n", entry)
			fmt.Println(entry.Name)
			fmt.Println(entry.Info)
		}
	}()

	// Start the lookup
	mdns.Lookup("谁还没有个 MacBook Pro 呢", entriesCh)
	time.Sleep(3 * time.Second)
	close(entriesCh)
}
