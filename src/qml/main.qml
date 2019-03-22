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
    property int realWidth : completerEdit.width + settingsButton.width + addButton.width + 20
    width: realWidth + (valueBalloon.visible ? (valueBalloon.width + 20) : 0)
    height: completerEdit.height
    x: (screen.width - realWidth) / 2
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

        onValueLoaded: {
            if (completerEdit.needInsert) {
                completerEdit.needInsert = false
                worker.insertValue(value)
                return
            }

            valueBalloon.text = value
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

        anchors.left: completerEdit.right
        anchors.leftMargin: 10
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
        anchors.leftMargin: 10

        onNeedClearValueInfo: {
            valueBalloon.visible = !hide
            valueBalloon.text = ""
        }
    }

    ValueBalloon {
        id: valueBalloon
        width : completerEdit.width / 2
        height: realHeight / 1.5
        anchors.left: addButton.right
        y: (realHeight - height) / 2
        anchors.leftMargin: 10

        BusyIndicator {
            visible: valueBalloon.text.length === 0
            anchors.right: parent.right
            anchors.margins: 3
            height: parent.height - 3
            width: height
            running: true
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
