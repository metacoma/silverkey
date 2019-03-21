import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import Qt.labs.platform 1.1

Window {
    id: mainWindow

    visible: false
    flags: Qt.FramelessWindowHint | Qt.Tool | Qt.Dialog | Qt.WA_TranslucentBackground
    color: "transparent"

    readonly property int realHeight: 80

    width: screen.width / 3
    height: completerEdit.height
    x: (screen.width - width) / 2
    y: screen.height / 2 - realHeight
    title: qsTr("SilverKey")

    Connections {
        target: worker
        onHideWindow: mainWindow.hide()


        onShowErrorMessage: {
            messageDialog.setDetailedText(error)
            messageDialog.visible = true
        }


        onRaiseWindow: {
            mainWindow.show()
            mainWindow.raise()
        }
    }

    ToolButton {
        id: settingsButton

        anchors.left: parent.left
        y: (realHeight - height) / 2

        width: height
        height: realHeight / 2

        icon.source: "qrc:/images/gear.png"
        icon.width: 32
        icon.height: 32
        icon.color: "transparent"
        onClicked: settingsDialog.open()

    }

    ToolButton {
        id: addButton

        anchors.right: parent.right
        y: (realHeight - height) / 2

        width: height
        height: realHeight / 2

        icon.source: "qrc:/images/add.png"
        icon.width: 32
        icon.height: 32
        icon.color: "transparent"

        onClicked: insertDataDialog.open()
    }

    CompleterEdit {
        id: completerEdit

        anchors.top: parent.top
        anchors.left: settingsButton.right
        anchors.right: addButton.left
        anchors.leftMargin: 10
        anchors.rightMargin: 10

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

    MessageDialog{
        id: messageDialog

        onAccepted: {
            messageDialog.close()
            settingsDialog.open()
        }
    }

    SettingsDialog {
        id: settingsDialog
    }

    InsertDataDialog {
        id: insertDataDialog
    }




}
