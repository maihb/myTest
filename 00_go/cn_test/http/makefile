#查找当前目录的.go文件作为main，也可以自定义
MAIN:=$(wildcard *.go)
#编译的目标程序名，默认为main文件的前缀
NAME:=$(basename $(MAIN))

#这里循环遍历目录的go文件
DIRS := $(shell find . -type d|grep -v '.git'|grep -v doc)
FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.go))

PACKAGE_NAME := createw.com/inteGrationHALService/fprModel
VERSION := `git describe --dirty`
COMMIT := `git rev-parse HEAD`


PLATFORM := linux
BUILD_DIR := ../../build
#VAR_SETTING := -X $(PACKAGE_NAME)/constant.Version=$(VERSION) -X $(PACKAGE_NAME)/constant.Commit=$(COMMIT)
GOBUILD = env CGO_ENABLED=1 $(GO_DIR)go build -buildmode=c-shared -tags "full" -trimpath -ldflags="-s -w -buildid= $(VAR_SETTING)" -o $(BUILD_DIR)

.PHONY: mac 
normal: mac

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f *.zip
	rm -f *.dat
	rm -f $(TEST_NAME)

test:
	env CGO_ENABLED=1 go build -tags "full" -trimpath -o $(TEST_NAME)

# test-arm:
# 	env CGO_ENABLED=1 GOARCH=arm64 GOOS=linux CC="zig cc -target aarch64-linux" CXX="zig c++ -target aarch64-linux"  go build -tags "full" -trimpath -o $(TEST_NAME)

mac:$(FILES)
	@mkdir -p $(BUILD_DIR)/$@
	go build -o $(BUILD_DIR)/$@

linux-amd64:$(FILES)
	@mkdir -p $(BUILD_DIR)/$@
	GOARCH=amd64 GOOS=linux go build -o $(BUILD_DIR)/$@

linux-armv8:
	@echo "交叉编译失败，暂时放弃"
#	mkdir -p $(BUILD_DIR)/$@
#	GOARCH=arm64 GOOS=linux CC="zig cc -target aarch64-linux" CXX="zig c++ -target aarch64-linux" $(GOBUILD)/$@
#	@mv $(BUILD_DIR)/$@/$(NAME) $(BUILD_DIR)/$@/$(LINUX_NAME)
#	@rm $(BUILD_DIR)/$@/$(NAME).h

linux-mips64:$(FILES)
	@echo "交叉编译失败，暂时放弃"
#	@mkdir -p $(BUILD_DIR)/$@
#	GOARCH=mips64 GOOS=linux  $(GOBUILD)/$@
#	@mv $(BUILD_DIR)/$@/$(NAME) $(BUILD_DIR)/$@/$(LINUX_NAME)
