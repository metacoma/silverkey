import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5

Window {
    visible: true
    flags: Qt.FramelessWindowHint | Qt.Tool | Qt.Dialog | Qt.WA_TranslucentBackground
    color: "transparent"

    readonly property int realHeight: 80

    width: screen.width / 3
    height: !suggestions.visible ? realHeight : realHeight + suggestions.height
    x: (screen.width - width) / 2
    y: screen.height / 2 - realHeight
    title: qsTr("SilverKey")

    Rectangle {
        id: inputContainer
        color: "white"
        radius: 5
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
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

            onAccepted: suggestions.visible = false
        }
    }

    Rectangle {
        id: suggestions
        radius: 5
        height: 400
        anchors.top: inputContainer.bottom
        anchors.topMargin: 2
        anchors.left: inputContainer.left
        anchors.right: inputContainer.right
        visible: false

        ListView {
            anchors.fill: parent
            anchors.margins: 5

            model: worker.keysModel

            delegate: Label {
                font.pointSize: 20
                textFormat: Text.RichText
                text: "<span style=\"background-color: #8080ff\">" + mainInput.text
                      + "</span>" + (model && display ? display.substring(mainInput.text.length) : "")
            }
        }
    }
}
