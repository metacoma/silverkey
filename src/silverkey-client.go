package main

import (
	"log"
  "os"

	"github.com/octoblu/go-simple-etcd-client/etcdclient"
	"github.com/wneo/jlfuzzy"

)

var ETCD_SERVER = os.Getenv("SILVERKEY_HOST")

func main() {
	fuzzy := jlfuzzy.NewJLFuzzy()

  client, err := etcdclient.Dial(ETCD_SERVER)

	if err != nil {
		log.Fatal(err)
  }

  nodes, err := client.LsRecursive("/")
	fuzzy.AddWords(nodes)

	if err != nil {
		log.Fatal(err)
  }

	/* UI part */

	for _, match := range fuzzy.SearchWord(os.Args[1], 1, -1, 0, 10) {
		log.Printf("Match %v\n", match)
	}


}
