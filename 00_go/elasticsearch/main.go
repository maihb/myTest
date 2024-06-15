package main

import (
	"context"
	"encoding/json"
	"fmt"
	"io"
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
	if err != nil {
		log.Fatalf("Error creating the client: %s", err)
	}

	// 构建 Bulk 请求体
	var b strings.Builder

	// 索引操作
	meta := []byte(`{ "index" : { "_index" : "abc", "_id" : "1" } }` + "\n")
	data := []byte(`{ "field1" : "value1" }` + "\n")
	b.Grow(len(meta) + len(data))
	b.Write(meta)
	b.Write(data)

	// 更新操作
	meta = []byte(`{ "update" : { "_index" : "abc", "_id" : "1" } }` + "\n")
	data = []byte(`{ "doc" : { "field2" : "value2" } }` + "\n")
	b.Grow(len(meta) + len(data))
	b.Write(meta)
	b.Write(data)

	// 删除操作
	meta = []byte(`{ "delete" : { "_index" : "abc", "_id" : "1" } }` + "\n")
	b.Grow(len(meta))
	b.Write(meta)

	// 创建操作
	meta = []byte(`{ "create" : { "_index" : "abc", "_id" : "2" } }` + "\n")
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
	body, _ := io.ReadAll(res.Body)
	//log.Println("body=", string(body))

	// 解析响应
	var r map[string]interface{}
	if err := json.Unmarshal(body, &r); err != nil {
		log.Fatalf("Error parsing the response body: %s", err)
	}
	// 打印响应结果
	jsondata, err := json.MarshalIndent(r, " ", " ")
	log.Printf("Bulk err=%v Response: \n%s", err, string(jsondata))
	//fmt.Printf("%s\n", r)

	test1(es)

}

// 定义一个结构体来表示你的数据
type Tweet struct {
	User    string `json:"user"`
	Message string `json:"message"`
}

// 另外一个测试用例
func test1(es *elasticsearch.Client) {
	if es == nil {
		return
	}
	// 插入数据
	tweet := Tweet{User: "olivere", Message: "Take Five"}
	data, err := json.Marshal(tweet)
	if err != nil {
		log.Fatalf("Error marshaling document: %s", err)
	}

	res, err := es.Index(
		"twitter",
		strings.NewReader(string(data)),
		es.Index.WithDocumentID("1"),
		es.Index.WithRefresh("true"),
	)
	if err != nil {
		log.Fatalf("Error indexing document: %s", err)
	}
	defer res.Body.Close()
	fmt.Printf("Indexed document %s to index %s\n", res.String(), "twitter")

	// 查询数据
	query := `{
        "query": {
            "term": {
                "user": {
                    "value": "olivere"
                }
            }
        }
    }`

	searchRes, err := es.Search(
		es.Search.WithContext(context.Background()),
		es.Search.WithIndex("twitter"),
		es.Search.WithBody(strings.NewReader(query)),
		es.Search.WithTrackTotalHits(true),
		es.Search.WithPretty(),
	)
	if err != nil {
		log.Fatalf("Error getting response: %s", err)
	}
	defer searchRes.Body.Close()

	if searchRes.IsError() {
		log.Fatalf("Error: %s", searchRes.String())
	}

	var searchResult map[string]interface{}
	if err := json.NewDecoder(searchRes.Body).Decode(&searchResult); err != nil {
		log.Fatalf("Error parsing the response body: %s", err)
	}

	fmt.Printf("Query took %d milliseconds\n", int(searchResult["took"].(float64)))
	fmt.Printf("Found a total of %d tweets\n", int(searchResult["hits"].(map[string]interface{})["total"].(map[string]interface{})["value"].(float64)))

	for _, hit := range searchResult["hits"].(map[string]interface{})["hits"].([]interface{}) {
		source := hit.(map[string]interface{})["_source"]
		user := source.(map[string]interface{})["user"]
		message := source.(map[string]interface{})["message"]
		fmt.Printf("Tweet by %s: %s\n", user, message)
	}

	// 删除数据
	deleteRes, err := es.Delete(
		"twitter",
		"1",
		es.Delete.WithRefresh("true"),
	)
	if err != nil {
		log.Fatalf("Error deleting document: %s", err)
	}
	defer deleteRes.Body.Close()
	fmt.Printf("Deleted document %s from index %s\n", deleteRes.String(), "twitter")
}
