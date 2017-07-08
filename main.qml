import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.2

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("LView")


    TableView {
        id: tableView
        anchors.fill: parent
        model: myModel
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
