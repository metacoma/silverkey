package main

import (
  "log"
  "os"

  "github.com/octoblu/go-simple-etcd-client/etcdclient"
  "github.com/wneo/jlfuzzy"
	"github.com/andlabs/ui"

)

var ETCD_SERVER = os.Getenv("SILVERKEY_HOST")

func main() {
	err := ui.Main(func() {
		input := ui.NewEntry()
		button := ui.NewButton("Greet")
		greeting := ui.NewLabel("")
		box := ui.NewVerticalBox()
		box.Append(ui.NewLabel("Enter your name:"), false)
		box.Append(input, false)
		box.Append(button, false)
		box.Append(greeting, false)
		window := ui.NewWindow("Hello", 200, 100, false)
		window.SetMargined(true)
		window.SetChild(box)
		button.OnClicked(func(*ui.Button) {
			greeting.SetText("Hello, " + input.Text() + "!")
		})
		window.OnClosing(func(*ui.Window) bool {
			ui.Quit()
			return true
		})
		window.Show()
	})
	if err != nil {
		panic(err)
	}

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
