#这里循环遍历目录的go文件
DIRS := $(shell find . -type d|grep -v '.git'|grep -v doc)
FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.go))

BUILD_DIR := ./
GOBUILD = env CGO_ENABLED=1 $(GO_DIR)go build  

.PHONY: aaaaaaaaaaaa
normal: go

clean:
	rm -rf *.out

go:$(FILES)
	$(GOBUILD)

