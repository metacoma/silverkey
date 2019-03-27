import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5

Item {
    id: completerEdit

    property string text: mainInput.text
    property bool needInsert: false
    signal needClearValueInfo(bool hide)
    function giveFocus() {
        mainInput.forceActiveFocus()
    }

    height: inputContainer.height + (suggestions.visible ? (suggestions.height + 2) : 0)
    width: inputContainer.width

    Rectangle {
        id: inputContainer
        color: "white"
        radius: 5
        anchors.top: parent.top
        width: screen.width / 3
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        height: realHeight

        TextField {
            id: mainInput

            anchors.fill: parent
            anchors.margins: 5
            palette {
                highlight: "transparent"
                mid: "transparent"
            }

            placeholderText: qsTr("Enter the key")
            font.pointSize: 24

            onTextEdited:{
                worker.keysModel.setFilter(text)
                suggestions.visible = true
            }

            Keys.onUpPressed: {
                view.canChangeInput = true
                view.currentIndex = view.currentIndex > 0 ? view.currentIndex - 1 : view.count - 1
            }
            Keys.onDownPressed: {
                view.canChangeInput = true
                view.currentIndex = view.currentIndex < view.count - 1 ? view.currentIndex + 1 : 0
            }
            Keys.onEscapePressed: {
                mainInput.clear()
                worker.escapePressed()
                Window.window.hide()
            }
            onAccepted: {
                if (view.currentItem) {
                    completerEdit.needInsert = true
                    worker.getValue(view.currentItem.key);
                    suggestions.visible = false
                    view.canChangeInput = false
                    completerEdit.needClearValueInfo(false)
                    mainInput.clear()
                }
            }
        }
    }

    Rectangle {
        id: suggestions
        radius: 5
        height: 300
        anchors.top: inputContainer.bottom
        anchors.topMargin: 2
        anchors.left: inputContainer.left
        anchors.right: inputContainer.right
        visible: false

        ScrollView {
            anchors.fill: parent
            anchors.margins: 5
            ListView {
                id: view
                property bool canChangeInput: false
                anchors.left: parent.left
                anchors.right: parent.right
                model: worker.keysModel
                clip: true
                delegate: Label {
                    property string key: display
                    width: view.width
                    font.pointSize: 20
                    textFormat: Text.RichText
                    text: (model && display) ? ( key.startsWith(mainInput.text)
                                              ? ("<span style=\"background-color: #8080ff\">"
                                                 + key.substring(0, mainInput.text.length)
                                                 + "</span>"
                                                 + key.substring(mainInput.text.length))
                                              : key) : ""


                }

                highlight: Rectangle {
                    radius: 5
                    color: "lightgray"
                }

                onCurrentItemChanged: {
                    if (currentItem && canChangeInput) {
                        mainInput.text = currentItem.key
                        worker.getValue(view.currentItem.key);
                        completerEdit.needClearValueInfo(false)
                    } else {
                        completerEdit.needClearValueInfo(true)
                    }
                }
            }
        }
    }
}
