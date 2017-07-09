import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("LView")
    objectName:  "RootWindow"

    signal updateAllSignal(string msg)

    toolBar:ToolBar {
        //height: 50
        RowLayout {
            anchors.fill: parent

            ToolButton {
                height: parent.height
                iconSource: "qrc:/icons/update.svg"
                onClicked: {
                    console.log("on update all")
                    rootWindow.updateAllSignal("qml")
                }
            }
        }
    }
    Component {
        id: tabTemplate

        Item {
        }
    }

    Component {
        id: viewTemplate
        LogViewer {
            id: tabView
            anchors.fill: parent
        }
    }

    function addTab(tabTitle, model){
        console.log("add tab", tabTitle, "model", model)
        var tab=tabView.addTab(tabTitle,tabTemplate)
        tab.active=true
        var obj=viewTemplate.createObject(tab.item,{logModel:model})
    }

    TabView{
        id: tabView
        anchors.fill: parent
    }
}
