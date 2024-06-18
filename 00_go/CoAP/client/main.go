package main

import (
	"context"
	"crypto/tls"
	"crypto/x509"
	"io"
	"log"
	"os"
	"time"

	piondtls "github.com/pion/dtls/v2"
	"github.com/plgd-dev/go-coap/v3/dtls"
	"github.com/plgd-dev/go-coap/v3/message/pool"
)

func createClientConfig(_ context.Context) (*piondtls.Config, error) {

	certificate, err := tls.LoadX509KeyPair("../ssl/client_cert.pem", "../ssl/client_key.pem")
	if err != nil {
		log.Fatalf("Failed to load certificates: %v", err)
	}

	// Load root CA certificate
	caCert, err := os.ReadFile("../ssl/root_ca_cert.pem")
	if err != nil {
		log.Fatalf("Failed to read CA certificate: %v", err)
	}

	certPool := x509.NewCertPool()
	if !certPool.AppendCertsFromPEM(caCert) {
		log.Fatalf("Failed to append CA certificate to pool")
	}

	return &piondtls.Config{
		Certificates:         []tls.Certificate{certificate},
		ExtendedMasterSecret: piondtls.RequireExtendedMasterSecret,
		RootCAs:              certPool,
		//InsecureSkipVerify:   true,
	}, nil
}

func main() {
	config, err := createClientConfig(context.Background())
	if err != nil {
		log.Fatalln(err)
		return
	}
	co, err := dtls.Dial("localhost:5688", config)
	if err != nil {
		log.Fatalf("Error dialing: %v", err)
	}
	log.Println("connect succed")
	// path := "/a"
	// if len(os.Args) > 1 {
	// 	path = os.Args[1]
	// }

	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()
	// resp, err := co.Get(ctx, path)
	// if err != nil {
	// 	log.Fatalf("Error sending request: %v", err)
	// }
	// log.Printf("Response payload: %+v", resp)

	//读取服务器主动下发消息
	num := 0
	sync := make(chan bool)
	go func() {
		time.Sleep(time.Second)
		_, err := co.Observe(ctx, "/register", func(req *pool.Message) {
			num++
			data, err := io.ReadAll(req.Body())
			log.Printf("num[%d]Got %+v, Payload=[%s], err=%v\n", num, req, string(data), err)
			if num == 2 {
				log.Println("end  sync.........")
				sync <- true
				log.Println("sync sended")
			}
		})
		log.Println("-------------------------- co.obs ----------------")
		if err != nil {
			log.Fatalf("Unexpected error '%v'", err)
		}
	}()

	log.Println("waite sync....")
	<-sync
	co.Close()
	time.Sleep(2 * time.Second)
}
