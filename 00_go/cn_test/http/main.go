package main

import (
	"fmt"
	"log"
	"net/http"
	"os"
	"strings"
)

var addr = ":8086"

func main() {
	svr := &server{}
	if len(os.Args) > 1 {
		addr = fmt.Sprintf(":%s", os.Args[1])
	}
	if len(os.Args) > 2 {
		svr.setRemoteUrl(os.Args[2])
	}
	log.Printf("http server begin on addr %s\n", addr)
	server_http := &http.Server{
		Handler: svr,
		// ReadTimeout:  10 * time.Second,
		// WriteTimeout: 10 * time.Second,
		Addr: addr,
	}
	if strings.Contains(addr, "443") {
		log.Fatal(server_http.ListenAndServeTLS("ssl.crt", "ssl.key"))
	} else {
		log.Fatal(server_http.ListenAndServe())
	}
}
