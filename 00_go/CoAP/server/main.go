package main

import (
	"bytes"
	"context"
	"crypto/tls"
	"crypto/x509"
	"fmt"
	"log"
	"math/big"
	"os"
	"sync"
	"time"

	piondtls "github.com/pion/dtls/v2"
	"github.com/plgd-dev/go-coap/v3/dtls"
	"github.com/plgd-dev/go-coap/v3/message"
	"github.com/plgd-dev/go-coap/v3/message/codes"
	"github.com/plgd-dev/go-coap/v3/mux"
	"github.com/plgd-dev/go-coap/v3/net"
	"github.com/plgd-dev/go-coap/v3/options"
	"github.com/plgd-dev/go-coap/v3/udp/client"
)

func onNewConn(cc *client.Conn) {
	dtlsConn, ok := cc.NetConn().(*piondtls.Conn)
	if !ok {
		log.Fatalf("invalid type %T", cc.NetConn())
	}
	clientCert, err := x509.ParseCertificate(dtlsConn.ConnectionState().PeerCertificates[0])
	if err != nil {
		log.Fatal(err)
	}
	cc.SetContextValue("client-cert", clientCert)
	cc.AddOnClose(func() {
		log.Println("closed connection")
	})
}

var (
	observers = sync.Map{}
)

func toHexInt(n *big.Int) string {
	return fmt.Sprintf("%x", n) // or %X or upper case
}

func handleA(w mux.ResponseWriter, r *mux.Message) {
	clientCert := r.Context().Value("client-cert").(*x509.Certificate)
	log.Println("Serial number:", toHexInt(clientCert.SerialNumber))
	log.Println("Subject:", clientCert.Subject)
	log.Println("Email:", clientCert.EmailAddresses)

	log.Printf("got message in handleA:  %+v from %v\n", r, w.Conn().RemoteAddr())
	err := w.SetResponse(codes.GET, message.TextPlain, bytes.NewReader([]byte("A hello world")))
	if err != nil {
		log.Printf("cannot set response: %v", err)
	}
}

func handleObs(w mux.ResponseWriter, r *mux.Message) {
	clientCert := r.Context().Value("client-cert").(*x509.Certificate)
	log.Println("handleObs ---------- Serial number:", toHexInt(clientCert.SerialNumber))
	for i := 0; i < 20; i++ {
		log.Println("for i=", i)
		cc := w.Conn()
		m := cc.AcquireMessage(cc.Context())
		//defer cc.ReleaseMessage(m)
		m.SetCode(codes.Content)
		m.SetToken(r.Token())
		m.SetBody(bytes.NewReader([]byte(fmt.Sprintf("Been running%d for %v", i, 123))))
		m.SetContentFormat(message.TextPlain)
		obs := 2 + i
		//m.SetObserve(uint32(obs))
		err := cc.WriteMessage(m)
		log.Printf("cc.WriteMessage(m) obs[%d] err=%v\n", obs, err)
		if err != nil {
			break
		}
		time.Sleep(300 * time.Millisecond)
	}
}
func main() {
	m := mux.NewRouter()
	m.Handle("/a", mux.HandlerFunc(handleA))
	m.Handle("/register", mux.HandlerFunc(handleObs))

	config, err := createServerConfig(context.Background())
	if err != nil {
		log.Fatalln(err)
		return
	}
	log.Println("begin....")
	log.Fatal(listenAndServeDTLS("udp", ":5688", config, m))
}

func listenAndServeDTLS(network string, addr string, config *piondtls.Config, handler mux.Handler) error {
	l, err := net.NewDTLSListener(network, addr, config)
	if err != nil {
		return err
	}
	defer l.Close()
	s := dtls.NewServer(options.WithMux(handler), options.WithOnNewConn(onNewConn))
	return s.Serve(l)
}

func createServerConfig(ctx context.Context) (*piondtls.Config, error) {
	certificate, err := tls.LoadX509KeyPair("../ssl/server_cert.pem", "../ssl/server_key.pem")
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
		ClientCAs:            certPool,
		ClientAuth:           piondtls.RequireAndVerifyClientCert,
		ConnectContextMaker: func() (context.Context, func()) {
			return context.WithTimeout(ctx, 30*time.Second)
		},
	}, nil
}
