import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.3 as OldControls
import QtQuick.Controls 2.5

OldControls.Dialog {
    id: insertDataDialog
    title: "Insert Data"

    width: screen.width / 3
    height: 400

    contentItem: Item {
        id: content
        width: insertDataDialog.width / 2
        height: width

        Column {
            anchors.fill: parent
            spacing: 10
            ScrollView {
                anchors.left: parent.left
                anchors.right: parent.right
                height: insertDataDialog.height - buttonsRow.height * 1.2
                TextArea {
                    id: dataArea

                }

            }
            Row {
                id: buttonsRow
                anchors.left: parent.left
                anchors.right: parent.right

                spacing: 10

                Button {
                    text: qsTr("Save")
                    onClicked: {
                        worker.insertData(dataArea.text)
                        insertDataDialog.close()
                    }
                }

                Button {
                    text: qsTr("Cancel")
                    onClicked: insertDataDialog.close()
                }
            }
        }
    }
}
