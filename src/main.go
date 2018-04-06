package main

import (

  _"net/http"
  _"net"
  "os"
  "log"
  "fmt"
  "net/url"
	"time"


  "github.com/octoblu/go-simple-etcd-client/etcdclient"
  "github.com/wneo/jlfuzzy"
  "github.com/zserge/webview"
  "github.com/go-vgo/robotgo"

)

type Nodes struct {
	Value string `json:"value"`
}


var fuzzy = jlfuzzy.NewJLFuzzy()
var nodes []string
var w webview.WebView

func (nodes *Nodes) Search(searchkey string) {
  nodes.Value = ""
  for _, match := range fuzzy.SearchWord(searchkey, 1, -1, 0, 10) {
    nodes.Value = nodes.Value + fmt.Sprintf("<option value=foobar>%s</option>", match)
  }
}

func (nodes *Nodes) Found(nodeName string) {
	w.Terminate();
}

//var ETCD_SERVER = os.Getenv("SILVERKEY_HOST")
var ETCD_SERVER = os.Getenv("http://nseha.linkpc.net:22379")
const myHTML = `<!doctype html>
	<html>
		<head>
    	<script type="text/javascript">
				function runScript(e) {
    			if (e.keyCode == 13) {
						external.invoke('close')
						//nodes.found("hello");
						//document.getElementById("test").innerHTML = e.keyCode;
    			}
				}
        function onInputChange(input) {
					nodes.search(input.value);
					document.getElementById("nodes").innerHTML = nodes.data.value;
        }
				function onLoad() {
					document.getElementById("test").innerHTML = "init";
				}
				window.onload = onLoad
      </script>
		</head>
		<body>
			<input type=text name=key oninput="onInputChange(this)" onkeypress="runScript(event)"></input>
			<br>
			<select id="nodes">
			</select>
			<div id=test></div>
		</body>
	</html>
`




func handleRPC(w webview.WebView, data string) {
	log.Printf("RCP %s\n", data)
	w.Terminate()
}

func main() {

  client, err := etcdclient.Dial(ETCD_SERVER)

  if err != nil {
    log.Fatal(err)
  }

  nodes, err := client.LsRecursive("/")
  fuzzy.AddWords(nodes)

  if err != nil {
    log.Fatal(err)
  }

	w = webview.New(webview.Settings{
		URL: `data:text/html,` + url.PathEscape(myHTML),
		Debug: true,
		ExternalInvokeCallback: handleRPC,
  })
	w.Dispatch(func() {
		w.Bind("nodes", &Nodes{})
	})
	w.Run()

  robotgo.TypeString("Hello World")

}
