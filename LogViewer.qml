import QtQuick 2.5
import QtQuick.Controls 1.2

Rectangle {
    property alias logModel: tableView.model

    Component {
        id: logLineDelegate
        Text{
            text: modelData
        }
    }

    Component {
        id: rowDelegate
        Item{
            width: parent.width
            height: 15*count
            Row {
                anchors.fill: parent
                spacing: 20

                Text {
                    id: titleColumn
                    text: timestamp
                    elide: Text.ElideRight
                }

                Column {
                    Repeater {
                        model: message
                        delegate: logLineDelegate
                    }
                }
            }
        }

    }
    ScrollView{
        anchors.fill: parent
        ListView {
            id: tableView
            anchors.fill: parent
            delegate: rowDelegate
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            focus: true
            interactive: true
        }
    }
}

