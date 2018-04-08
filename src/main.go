package main

import (
	_"context"
  "net/http"
  _"net/url"
  "net"
  "log"
  "fmt"
  "mime"
  "os"
  "syscall"
  _"time"
  "io"
  "path/filepath"
  "bytes"
  "github.com/octoblu/go-simple-etcd-client/etcdclient"
  "github.com/wneo/jlfuzzy"
  "github.com/zserge/webview"
  "github.com/go-vgo/robotgo"
  "github.com/hidez8891/shm"
  _"github.com/coreos/etcd/client"

)

var fuzzy = jlfuzzy.NewJLFuzzy()
var nodes []string
//var ETCD_SERVER = os.Getenv("SILVERKEY_HOST")
var ETCD_SERVER = "http://nseha.linkpc.net:22379"
//var ETCD_SERVER = "http://hw2:2379"
var w webview.WebView
var answer_value = "404"

type Nodes struct {
	Value string `json:"value"`
}

type Answer struct {
	Value string `json:"value"`
}

func (nodes *Nodes) Search(searchkey string) {
  nodes.Value = ""
  for _, match := range fuzzy.SearchWord(searchkey, 1, -1, 0, 10) {
    nodes.Value = nodes.Value + fmt.Sprintf("<div class=divTableRow><div class=divTableCell>%s</div></div>", match)
  }
}

func (answer *Answer) SetValue(value string) {
    answer_value = value
}

func (nodes *Nodes) Found(nodeName string) {
	w.Terminate();
}

func handleRPC(w webview.WebView, data string) {
	log.Printf("RCP %s\n", data)

  m, _ := shm.Open(SHMEM_KEY, 256)

  wbuf := make([]byte, 256)
  for i := range wbuf {
    wbuf[i] = 0
  }

  copy(wbuf[:], answer_value)
  m.Write(wbuf)

  m.Close()

	w.Terminate()
}

func startServer() string {
	ln, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		log.Fatal(err)
	}
	go func() {
		defer ln.Close()
		http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
			path := r.URL.Path
			if len(path) > 0 && path[0] == '/' {
				path = path[1:]
			}
			if path == "" {
				path = "index.html"
			}
			if bs, err := Asset(path); err != nil {
				w.WriteHeader(http.StatusNotFound)
			} else {
				w.Header().Add("Content-Type", mime.TypeByExtension(filepath.Ext(path)))
				io.Copy(w, bytes.NewBuffer(bs))
			}
		})
		log.Fatal(http.Serve(ln, nil))
	}()
	return "http://" + ln.Addr().String()
}

const (
  CHILD_ARG = "child"
  SHMEM_KEY = "silverkey"
)


func main() {

    if len(os.Args) >= 2 && os.Args[1] == CHILD_ARG {

		  println("child RUN")
      client_etcd, err := etcdclient.Dial(ETCD_SERVER)
      if err != nil {
        log.Fatal(err)
      }

      nodes, err := client_etcd.LsRecursive("/")
      fuzzy.AddWords(nodes)

      if err != nil {
        log.Fatal(err)
      }
      url := startServer()

      w = webview.New(webview.Settings{
		    URL: url,
		    Width:  320,
		    Height: 200,
		    Debug: true,
		    ExternalInvokeCallback: handleRPC,
      })
	    w.Dispatch(func() {
		    w.Bind("nodes", &Nodes{})
		    w.Bind("answer", &Answer{})
	    })
	    w.Run()
      return
    }

    r, _ := shm.Create(SHMEM_KEY, 256)
    defer r.Close()

		println("parent RUN")
    pid, err := syscall.ForkExec(os.Args[0], []string{os.Args[0], CHILD_ARG}, &syscall.ProcAttr{Env: os.Environ(), Files: []uintptr{0, 1, 2}})

		if err != nil {
			panic(err.Error())
		}
		proc, err := os.FindProcess(pid)
		if err != nil {
			panic(err.Error())
		}



		state, err := proc.Wait()

		if err != nil {
			panic(err.Error())
		}

    println("pid:", state.Pid())
    println("Parent dies now.")


    rbuf := make([]byte, 256)
    _, err = r.Read(rbuf)
    if err != nil {
      log.Fatal(err)
    }

    client_etcd, err := etcdclient.Dial(ETCD_SERVER)
    if err != nil {
      log.Fatal(err)
    }

		etcd_key := fmt.Sprintf("%s", string(bytes.TrimRight(rbuf, "\x00")))
    log.Printf("Lookup key: %s (%d)\n", etcd_key, len(etcd_key))
    value, err := client_etcd.Get(etcd_key)
    if err != nil {
      log.Fatal(err)
    }
    log.Printf("Lookup key: %s, value: %s\n", etcd_key, value)


    robotgo.TypeString(value)

}
