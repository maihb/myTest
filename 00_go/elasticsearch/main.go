package main

import (
	"context"
	"encoding/json"
	"fmt"
	"log"
	"strings"
	"time"

	"github.com/cenkalti/backoff/v4"
	"github.com/elastic/go-elasticsearch/v8"
)

func main() {
	retryBackoff := backoff.NewExponentialBackOff()
	log.Println("begin....")
	es, err := elasticsearch.NewClient(elasticsearch.Config{
		Addresses: []string{"https://myes:9200"},
		//kibana 网页生成
		APIKey:   "RHlDekdaQUJLellJaUJMY1J3Wno6WEpWQ3ZmbzhTZy1DREtmbkVwcVI0Zw==",
		Username: "elastic",
		Password: "ShQoF9F_-H99OmWxBDKT",

		//  openssl x509 -in http_ca.crt -noout -fingerprint -sha256
		CertificateFingerprint: "1E58903BF642C53620EC99333A592E98F269E6316596407F48B86F74DB9F95C7",
		// Retry on 429 TooManyRequests statuses
		//
		RetryOnStatus: []int{502, 503, 504, 429},

		RetryBackoff: func(i int) time.Duration {
			log.Println("retry....", i)
			if i == 1 {
				retryBackoff.Reset()
			}
			return retryBackoff.NextBackOff()
		},
		MaxRetries: 1,
	})
	log.Println("eeeeeeeeeeeeerrrr")
	if err != nil {
		log.Fatalf("Error creating the client: %s", err)
	}
	log.Println("eeeeeeeeeeeeerrrr111")

	// 构建 Bulk 请求体
	var b strings.Builder

	// 索引操作
	meta := []byte(`{ "index" : { "_index" : "test", "_id" : "1" } }` + "\n")
	data := []byte(`{ "field1" : "value1" }` + "\n")
	b.Grow(len(meta) + len(data))
	b.Write(meta)
	b.Write(data)
	log.Println("eeeeeeeeeeeeerrrr111222")

	// 更新操作
	meta = []byte(`{ "update" : { "_index" : "test", "_id" : "1" } }` + "\n")
	data = []byte(`{ "doc" : { "field2" : "value2" } }` + "\n")
	b.Grow(len(meta) + len(data))
	b.Write(meta)
	b.Write(data)

	// 删除操作
	meta = []byte(`{ "delete" : { "_index" : "test", "_id" : "1" } }` + "\n")
	b.Grow(len(meta))
	b.Write(meta)

	// 创建操作
	meta = []byte(`{ "create" : { "_index" : "test", "_id" : "2" } }` + "\n")
	data = []byte(`{ "field1" : "value3" }` + "\n")
	b.Grow(len(meta) + len(data))
	b.Write(meta)
	b.Write(data)

	// 执行 Bulk 请求
	res, err := es.Bulk(
		strings.NewReader(b.String()),
		es.Bulk.WithContext(context.Background()),
	)
	if err != nil {
		log.Fatalf("Error getting response: %s", err)
	}
	defer res.Body.Close()

	// 解析响应
	var r map[string]interface{}
	if err := json.NewDecoder(res.Body).Decode(&r); err != nil {
		log.Fatalf("Error parsing the response body: %s", err)
	}

	// 打印响应结果
	fmt.Printf("Bulk Response: %s\n", r)
}
