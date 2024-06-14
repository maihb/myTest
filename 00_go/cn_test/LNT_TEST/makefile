#查找当前目录的.go文件作为main，也可以自定义
MAIN:=$(wildcard lib*.go)
#编译的目标程序名，默认为main文件的前缀
NAME:=$(basename $(MAIN))
LINUX_NAME := $(NAME).so
WIN_NAME := $(NAME).DLL
TEST_NAME := testLib

#这里循环遍历目录的go文件
DIRS := $(shell find . -type d|grep -v '.git'|grep -v doc)
FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.go))

PACKAGE_NAME := createw.com/inteGrationHALService/fprModel
VERSION := `git describe --dirty`
COMMIT := `git rev-parse HEAD`

BUILD_DIR := ../build
#VAR_SETTING := -X $(PACKAGE_NAME)/constant.Version=$(VERSION) -X $(PACKAGE_NAME)/constant.Commit=$(COMMIT)
BUILD_LIB = -buildmode=c-shared -tags "full" -trimpath -ldflags="-s -w -buildid= $(VAR_SETTING)"
WIN_CGO = CGO_ENABLED=1 CC=i686-w64-mingw32-gcc CXX=i686-w64-mingw32-g++

CROS_BUILD_ARM64 = GOARCH=arm64 GOOS=linux CC="zig cc -target aarch64-linux" CXX="zig c++ -target aarch64-linux" 

.PHONY: 
normal: win

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f *.zip
	rm -f *.dat

test:
	go build -o /tmp/fff

# test-arm:
# 	env CGO_ENABLED=1 GOARCH=arm64 GOOS=linux CC="zig cc -target aarch64-linux" CXX="zig c++ -target aarch64-linux"  go build -tags "full" -trimpath -o $(TEST_NAME)


#Linux交叉编译Windows程序，执行前运行：sudo apt-get install mingw-w64
win:$(FILES)
	@mkdir -p $(BUILD_DIR)
	$(WIN_CGO) GOOS=windows GOARCH=386 go build -o $(BUILD_DIR)/lnt.exe
	-cp $(BUILD_DIR)/lnt.exe ~/Downloads/lnt/

