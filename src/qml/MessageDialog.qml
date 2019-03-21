import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.3 as OldControls

OldControls.MessageDialog{
    id: messageDialog

    title: "Error"
    icon: OldControls.StandardIcon.Question
    text: "Failed to load data from database"
    standardButtons: StandardButton.Abort | StandardButton.Open
    onRejected: Qt.quit()
}
