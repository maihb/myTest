package main

import (
	"encoding/xml"
	"fmt"
	"log"
	"os"
)

func main() {
	type Email struct {
		Where string `xml:"where,attr"` //Email: [{ gre@example.com} { gre@work.com}]
		Addr1 string
	}
	type Address struct {
		City, State string
	}
	type Result struct {
		XMLName xml.Name `xml:"Person"`
		Name    string   `xml:"FullName"`
		Phone   string
		Email   []Email
		Groups  []string `xml:"Group>Value"`
		Address
	}
	v := Result{Name: "none", Phone: "none"}

	data, err := os.ReadFile("a.txt")
	if err != nil {
		log.Printf("failed to read config file %s: %v", "a.txt", err)
		data = []byte("")
	}
	err = xml.Unmarshal([]byte(data), &v)
	if err != nil {
		fmt.Printf("error: %v", err)
		return
	}
	fmt.Printf("XMLName: %#v\n", v.XMLName)
	fmt.Printf("Name: %q\n", v.Name)
	fmt.Printf("Phone: %q\n", v.Phone)
	fmt.Printf("Email:[%v],---- [%v]\n", v.Email[0].Where, v.Email[0].Addr1)
	fmt.Printf("Groups: %v\n", v.Groups)
	fmt.Printf("Address: %v\n", v.Address)
}
