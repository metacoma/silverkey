import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.3 as OldControls
import QtQuick.Controls 2.5

OldControls.Dialog {
    id: settingsDialog
    title: "Settings"
    width: screen.width / 6
    height: 400

    contentItem: Item {

        property alias login: loginEdit.text
        property alias password: passwordEdit.text
        property alias host: hostEdit.text
        property alias port: portEdit.text

        Column {
            anchors.fill: parent
            spacing: 10
            TextField {
                id: loginEdit
                placeholderText: qsTr("Login")
            }
            TextField {
                id: passwordEdit
                placeholderText: qsTr("Password")
            }
            TextField {
                id: hostEdit
                placeholderText: qsTr("silverkey.app")
            }
            TextField {
                id: portEdit
                placeholderText: qsTr("2379")
            }

            Row {
                anchors.left: parent.left
                anchors.right: parent.right

                spacing: 10

                Button {
                    text: qsTr("Save")
                    onClicked: {
                        worker.saveSettings(contentItem.login,
                                            contentItem.password,
                                            contentItem.host,
                                            contentItem.port)
                        settingsDialog.close()
                    }
                }

                Button {
                    text: qsTr("Cancel")
                    onClicked: settingsDialog.close()
                }
            }
        }
    }
}
