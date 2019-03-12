import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.3 as OldControls
import QtQuick.Controls 2.5 as NewControls
import Qt.labs.platform 1.1

Window {
    id: mainWindow

    visible: false
    flags: Qt.FramelessWindowHint | Qt.Tool | Qt.Dialog | Qt.WA_TranslucentBackground
    color: "transparent"

    readonly property int realHeight: 80

    width: screen.width / 3
    height: !suggestions.visible ? realHeight : realHeight + suggestions.height
    x: (screen.width - width) / 2
    y: screen.height / 2 - realHeight
    title: qsTr("SilverKey")

    Connections {
        target: worker
        onGotReplyFromDb: {
            mainWindow.hide()
        }

        onDataLoadError: {
            messageDialog.visible = true
        }

        onRaiseWindow: {
            mainWindow.show()
            mainWindow.raise()
        }
    }

    NewControls.ToolButton {
        id: settingsButton

        anchors.left: parent.left
        anchors.verticalCenter: inputContainer.verticalCenter

        width: height
        height: realHeight / 2

        icon.source: "qrc:/images/gear.png"
        icon.width: 32
        icon.height: 32
        icon.color: "transparent"

    }

    NewControls.ToolButton {
        id: addButton

        anchors.right: parent.right
        anchors.verticalCenter: inputContainer.verticalCenter

        width: height
        height: realHeight / 2

        icon.source: "qrc:/images/add.png"
        icon.width: 32
        icon.height: 32
        icon.color: "transparent"
    }

    Rectangle {
        id: inputContainer
        color: "white"
        radius: 5
        anchors.top: parent.top
        anchors.left: settingsButton.right
        anchors.right: addButton.left
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        height: realHeight

        NewControls.TextField {
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
                mainWindow.hide()
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
        height: 400
        anchors.top: inputContainer.bottom
        anchors.topMargin: 2
        anchors.left: inputContainer.left
        anchors.right: inputContainer.right
        visible: false

        ListView {
            id: view
            anchors.fill: parent
            anchors.margins: 5

            model: worker.keysModel

            delegate: NewControls.Label {
                property string key: display
                anchors.left: parent.left
                anchors.right: parent.right
                font.pointSize: 20
                textFormat: Text.RichText
                text: "<span style=\"background-color: #8080ff\">" + mainInput.text
                      + "</span>" + (model && display ? display.substring(mainInput.text.length) : "")
            }

            highlight: Rectangle {
                color: "lightgray"
            }
        }
    }

    SystemTrayIcon {
        visible: true
        icon.source: "qrc:/images/tray.png"

        menu: Menu {
            MenuItem {
                text: qsTr("Show")
                onTriggered: {
                    mainWindow.show()
                    mainWindow.raise()
                }
            }
            MenuItem {
                text: qsTr("Hide")
                onTriggered: {
                    mainWindow.hide()
                }
            }
            MenuSeparator {}
            MenuItem {
                text: qsTr("Quit")
                onTriggered: Qt.quit()
            }
        }
    }

    OldControls.MessageDialog{
        id: messageDialog

        title: "Error"
        icon: OldControls.StandardIcon.Question
        text: "Failed to load data from database"
        standardButtons: StandardButton.Abort | StandardButton.Open
        onRejected: Qt.quit()

        onAccepted: {
            visible = false
            settingsDialog.visible = true
        }
    }


    OldControls.Dialog {
        id: settingsDialog
        title: "Settings"

        width: mainWindow.width / 2
        height: width

        standardButtons: OldControls.StandardButton.Save | OldControls.StandardButton.Cancel

        onAccepted: {
            worker.saveSettings(contentItem.login,
                                contentItem.password,
                                contentItem.host,
                                contentItem.port)
            close()
        }

        onRejected: {
            close()
        }

        contentItem: Item {
            anchors.fill: parent
            property alias login: loginEdit.text
            property alias password: passwordEdit.text
            property alias host: hostEdit.text
            property alias port: portEdit.text

            Column {
                spacing: 10
                NewControls.TextField {
                    id: loginEdit
                    placeholderText: qsTr("Login")
                }
                NewControls.TextField {
                    id: passwordEdit
                    placeholderText: qsTr("Password")
                }
                NewControls.TextField {
                    id: hostEdit
                    placeholderText: qsTr("silverkey.app")
                }
                NewControls.TextField {
                    id: portEdit
                    placeholderText: qsTr("2379")
                }
            }

        }

    }
}
