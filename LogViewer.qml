import QtQuick 2.0
import QtQuick.Controls 1.2

Item {
    property alias logModel: tableView.model

    TableView {
        id: tableView
        anchors.fill: parent

        TableViewColumn {
            id: titleColumn
            title: qsTr("TimeStamp")
            role: "timestamp"
            movable: false
            resizable: false
            width: tableView.viewport.width /4
        }

        TableViewColumn {
            id: authorColumn
            title: qsTr("Message")
            role: "message"
            movable: false
            resizable: false
            width: tableView.viewport.width / 4*3
        }
    }
}
