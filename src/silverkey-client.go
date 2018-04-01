package main

import (
  "log"
  "os"

  "github.com/octoblu/go-simple-etcd-client/etcdclient"
  "github.com/wneo/jlfuzzy"

  "github.com/therecipe/qt/widgets"
  "github.com/therecipe/qt/gui"
  "github.com/therecipe/qt/core"
)

var ETCD_SERVER = os.Getenv("SILVERKEY_HOST")

func run_ui() {
    // Create application
    app := widgets.NewQApplication(len(os.Args), os.Args)

    // Create main window
    window := widgets.NewQMainWindow(nil, 0)
    // Remove title bar from window
    window.SetWindowFlags(core.Qt__Window|core.Qt__FramelessWindowHint)
    window.SetMinimumSize2(200, 200)
    // Register KeyPress event
    window.ConnectKeyPressEvent(func(event *gui.QKeyEvent) {
        key := event.Key()
        // If user pressed Return key close UI part
        if key == 16777220 {
            app.QuitDefault()
        } else {
            log.Printf("Entered Key: %v\n", key)
        }
    })

    // Create main layout
    layout := widgets.NewQVBoxLayout()

    // Create main widget and set the layout
    mainWidget := widgets.NewQWidget(nil, 0)
    mainWidget.SetLayout(layout)

    // Create a line edit and add it to the layout
    input := widgets.NewQLineEdit(nil)
    input.SetPlaceholderText("1. write something")
    layout.AddWidget(input, 0, 0)

    // Create list view for search result
    resultList := widgets.NewQListView(nil)
    data := []string{"as","asd","asd"}
    resultList.SetModel(core.NewQStringListModel2(data, nil))
    layout.AddWidget(resultList, 0, 0)

    // Set main widget as the central widget of the window
    window.SetCentralWidget(mainWidget)

    // Show the window
    window.Show()

    // Execute app
    app.Exec()
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

  /* UI part */

  for _, match := range fuzzy.SearchWord(os.Args[1], 1, -1, 0, 10) {
    log.Printf("Match %v\n", match)
  }


}
