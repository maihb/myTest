GO_TEST_CMD = $(if $(shell which richgo),richgo test,go test)
export ELASTICSEARCH_URL=http://elastic:elastic@localhost:9200
default:
	go build -o es_test
amd:
	GOARCH=amd64 GOOS=linux go build -o ../build/es_test.amd64
arm:
	GOARCH=arm64 GOOS=linux go build -o ../build/es_test.arm64
test: test-default test-indexer

test-default:
	go run default.go

test-indexer:
	go run indexer.go

test-benchmarks: clean setup
	cd benchmarks && go run benchmarks.go

setup:
	go install github.com/mailru/easyjson/easyjson@latest
	cd benchmarks && go generate ./model

clean:
	@rm -f benchmarks/model/*_easyjson.go

.PHONY: default amd arm test test-default test-indexer test-benchmarks setup clean
