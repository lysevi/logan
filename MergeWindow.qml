import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Layouts 1.0

Window {
    id: windowId
    title: "MergeWindow"
    width: 300
    height: 200

    property var logsMap: null
    property var logs: []

    Component.onCompleted: {
        console.log(">>> ",logsMap)
        for(var key in logsMap){
            logs.push(logsMap[key])
        }
        listView.model=logs
        console.log(listView.model)
    }


    ListView {
        id: listView
        anchors.fill: parent
        delegate: Text {
            text: modelData.name
        }
    }
}
