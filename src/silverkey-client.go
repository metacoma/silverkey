package main

import (
  "log"
  "os"

  "github.com/octoblu/go-simple-etcd-client/etcdclient"
  "github.com/wneo/jlfuzzy"
	"github.com/andlabs/ui"

)

var ETCD_SERVER = os.Getenv("SILVERKEY_HOST")
var MAX_RESULT = 10

func main() {
  var i int;
  fuzzy := jlfuzzy.NewJLFuzzy()

  client, err := etcdclient.Dial(ETCD_SERVER)

  if err != nil {
    log.Fatal(err)
  }

  nodes, err := client.LsRecursive("/")

	if err != nil {
		panic(err)
	}

  fuzzy.AddWords(nodes)

	err = ui.Main(func() {
		input := ui.NewEntry()
    combobox := ui.NewCombobox();


		box := ui.NewVerticalBox()
		box.Append(input, false)
    box.Append(combobox, false)
		window := ui.NewWindow("Hello", 400, 200, false)
		window.SetMargined(true)
		window.SetChild(box)

		window.OnClosing(func(*ui.Window) bool {
			ui.Quit()
			return true
		})

    input.OnChanged(func(*ui.Entry) {
      box.Delete(1)

      combobox.Destroy()

      combobox = ui.NewCombobox();
      i = 1
      for _, match := range fuzzy.SearchWord(input.Text(), 1, -1, 0, 10) {
        combobox.Append(match)
        i = i + 1
        if (i == MAX_RESULT) {
          break
        }
      }

      combobox.SetSelected(0)

      box.Append(combobox, false)


    })
		window.Show()
	})



  /* UI part */


}
