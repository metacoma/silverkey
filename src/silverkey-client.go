package main

import (
	"log"
  "os"

	"github.com/octoblu/go-simple-etcd-client/etcdclient"
)

var ETCD_SERVER = os.Getenv("SILVERKEY_HOST")

func main() {

  client, err := etcdclient.Dial(ETCD_SERVER)

	if err != nil {
		log.Fatal(err)
  }

  nodes, err := client.LsRecursive("/")

	if err != nil {
		log.Fatal(err)
  }

  for _, node := range nodes {
		log.Printf("Key %s\n", node)
  }

}
