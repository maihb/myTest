package main

/*如果struct{}的实例逃逸到heap上，那它们的地址可能相同。事实上，The Go Programming Language Specification: Size and alignment guarantees确认了：
 A struct or array type has size zero if it contains no fields (or elements, respectively) that have a size greater than zero. Two distinct zero-size variables may have the same address in memory.
 struct或者array如果其不包含size大于0的字段，则其size为0。两个不同的size为0的变量可能拥有同一个地址。
因此，emptyCtx类型不使用struct{}，为了确保todo和background拥有不同的地址。
*/

import (
	"log"
	"testing"
)

func TestMain(t *testing.T) {
	log.Println(convert("PAYPALISHIRING", 3))
}

func convert(s string, numRows int) string {
	l := len(s)
	wigh := 1
	mid := 0
	base := numRows
	if numRows > 2 {
		mid = numRows - 2
		base = numRows + mid
		wigh = 1 + mid
	}
	//wigh += (l / base) * (1 + mid)

	var res []byte
	for j := 0; j < numRows; j++ {
		for i := 0; i < (l/base+1)*wigh; i++ {
			y := i % wigh
			if y == 0 {
				index := i/wigh*base + j
				if index < l {
					res = append(res, s[index])
				}
			} else if (j + y) == (numRows - 1) {
				index := (i/wigh)*base + numRows + y
				log.Println("fffff", j, y, numRows, "xxx", index)
				if index < l {
					//res = append(res, s[index])
					res = append(res, ',')
				}
			}
		}
	}
	return string(res)
}
