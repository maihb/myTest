package main

import (
	"bytes"
	"fmt"
	"io"
	"log"
	"net/http"
	"net/http/httputil"
	"net/url"
)

type server struct {
	remote *url.URL
}

func (s *server) setRemoteUrl(addr string) {
	addr = fmt.Sprintf("http://%s", addr)
	log.Println("setRemoteUrl=", addr)
	s.remote, _ = url.Parse(addr)
}

func (s *server) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	// w.Header().Set("Access-Control-Allow-Origin", "*") //允许访问所有域
	// w.Header().Set("Access-Control-Allow-Methods", "OPTIONS,GET,POST,PUT,DELETE")
	// w.Header().Set("content-type", "application/json")  //返回数据格式是json
	// w.Header().Set("Access-Control-Allow-Headers", "*") //header的类型
	//w.Header().Add("Access-Control-Allow-Headers", "Content-Type") //header的类型

	body, err := io.ReadAll(r.Body)
	if err != nil {
		log.Println(err.Error())
		return
	}
	r.Body.Close()
	r.ParseForm()
	if r.FormValue("nolog") == "" {
		log.Printf("IP=%s,URL=%s, method=%s,body=%s,header=%v", r.RemoteAddr, r.URL.RequestURI(), r.Method, string(body), r.Header)
	}
	if s.remote != nil {
		// Because go lang is a pain in the ass if you read the body then any susequent calls
		// are unable to read the body again....
		r.Body = io.NopCloser(bytes.NewBuffer(body))
		s.redirectRequest(w, r)
	} else {

		io.WriteString(w, string(body))
	}
}

func (s *server) redirectRequest(w http.ResponseWriter, r *http.Request) {
	// queryForm := r.URL.Query()
	// queryForm.Set("nonce", nonce)
	// queryForm.Set("client_id", clientId)
	// queryForm.Set("timestamp", strconv.FormatInt(timestamp, 10))
	// queryForm.Set("signature", signature)
	// r.URL.RawQuery = queryForm.Encode()

	proxy := httputil.NewSingleHostReverseProxy(s.remote)
	proxy.ServeHTTP(w, r)
	return
}
