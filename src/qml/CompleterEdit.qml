import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5

Item {
    id: completerEdit

    height: inputContainer.height + (suggestions.visible ? (suggestions.height + 2) : 0)

    Rectangle {
        id: inputContainer
        color: "white"
        radius: 5
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        height: realHeight

        TextField {
            id: mainInput
            focus: true
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

            Keys.onUpPressed: view.currentIndex = view.currentIndex > 0 ? view.currentIndex - 1 : view.count - 1
            Keys.onDownPressed: view.currentIndex = view.currentIndex < view.count - 1 ? view.currentIndex + 1 : 0
            Keys.onEscapePressed: {
                mainInput.clear()
                worker.escapePressed()
                Window.window.hide()
            }
            onAccepted: {
                worker.insertValue(view.currentItem.key);
                suggestions.visible = false
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
                anchors.left: parent.left
                anchors.right: parent.right
                model: worker.keysModel
                clip: true
                delegate: Label {
                    property string key: display
                    width: view.width
                    font.pointSize: 20
                    textFormat: Text.RichText
                    text: "<span style=\"background-color: #8080ff\">" + (model && display ? display.substring(0, mainInput.text.length) : "")
                          + "</span>" + (model && display ? display.substring(mainInput.text.length) : "")
                }

                highlight: Rectangle {
                    color: "lightgray"
                }

                onCurrentItemChanged: {
                    if (currentItem)
                        mainInput.text = currentItem.key
                }
            }
        }
    }
}
