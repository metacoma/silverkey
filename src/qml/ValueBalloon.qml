import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5

Rectangle {
    id: balloon

    property alias text: label.text
    visible: false
    radius: height / 2
    color: "white"
    Label {
        id: label
        color: "black"
        anchors.fill: parent
        anchors.leftMargin: height / 2
        anchors.rightMargin: height / 2
        font.pointSize: 12
        horizontalAlignment: Qt.AlignLeft
        verticalAlignment: Qt.AlignVCenter
        clip: true
        elide: Text.ElideRight
        maximumLineCount: 1
    }
}
