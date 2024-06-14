package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"time"
)

const fileName = "printFile.pdf"

func main() {
	ip := "127.0.0.1"
	if len(os.Args) > 1 {
		ip = os.Args[1]
	}
	data := struct {
		FileExtent  string `json:"fileExtent"`
		TranType    int    `json:"tranType"`
		FileContent []byte `json:"fileContent"`
	}{
		FileExtent: "pdf",
		TranType:   1,
	}
	var err error
	data.FileContent, err = os.ReadFile(fileName)
	if err != nil {
		log.Printf("failed to read config file %s: %v", fileName, err)
		return
	}
	body, _ := json.Marshal(data)

	url := fmt.Sprintf("http://%s:7999/inteGration/hardwareControl/api/printNew", ip)
	req, err := http.NewRequest("POST", url, bytes.NewReader(body))
	if err != nil {
		log.Println(err)
		return
	}

	rsp, err := (&http.Client{
		Timeout: 10 * time.Minute,
	}).Do(req)
	if err != nil {
		log.Println(err)
		return
	}
	defer rsp.Body.Close()
	rspData, _ := io.ReadAll(rsp.Body)
	log.Printf("response: code[%d],body[%s]", rsp.StatusCode, string(rspData))
}
