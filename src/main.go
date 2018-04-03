package main

import (

  "net/http"
  "net"
  "os"
  "log"
  "fmt"


  "github.com/octoblu/go-simple-etcd-client/etcdclient"
  "github.com/wneo/jlfuzzy"
  "github.com/zserge/webview"
  "github.com/go-vgo/robotgo"

)

var ETCD_SERVER = os.Getenv("SILVERKEY_HOST")

func handler(w http.ResponseWriter, r *http.Request) {
      fmt.Fprintf(w, "<html><body><input type=text name=key></input><br><select name=nodes><option value=foobar>foobar</option></select></body></html>")
}

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
  robotgo.TypeString("Hello World")

      ln, err := net.Listen("tcp", "127.0.0.1:0")
        if err != nil {
                log.Fatal(err)
        }
        defer ln.Close()
        go func() {
                http.HandleFunc("/", handler)
                http.Serve(ln, nil)
        }()

        webview.Open("Silverkey", "http://"+ln.Addr().String()+"/", 400, 300, false)


  /* UI part */

  /*
  for _, match := range fuzzy.SearchWord(os.Args[1], 1, -1, 0, 10) {
    log.Printf("Match %v\n", match)
  }
  */


}
