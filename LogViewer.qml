import QtQuick 2.5
import QtQuick.Controls 1.2

Item {
    property alias logModel: tableView.model

    ScrollView{
        anchors.fill: parent
        ListView {
            id: tableView
            anchors.fill: parent

            delegate: Row {
                width: parent.width
                spacing: 20
                Text {
                    id: titleColumn
                    text: timestamp
                    elide: Text.ElideRight
                    anchors.rightMargin: 10
                }

                Column {
                    anchors.margins: 5
                    Repeater {
                        model: message

                        anchors {
                            fill: parent
                            margins: 2
                        }
                        delegate:/*Rectangle {
                            anchors {left: parent.left; right: parent.right; }
                            color: ( index % 2 == 0 ) ? "white" : "lightgrey"*/
                            Text{
                                text: modelData
//                                anchors.verticalCenter: parent.verticalCenter
//                                anchors.left: parent.left
                            }
                        //}
                    }
                }
            }
        }
    }
}

