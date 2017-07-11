import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("LView")
    objectName:  "RootWindow"

    signal updateAllSignal(string msg)
    signal openFileSignal(string fname)
    signal closeFileSignal(string fname)
    signal addHighlightedTextSignal(string str)
    signal clearHighlightedTextSignal()

    FileDialog {
        id: openFileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        onAccepted: {
            var path = openFileDialog.fileUrl.toString();
            // remove prefixed "file:///"
            path= path.replace(/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,"");
            // unescape html codes like '%23' for '#'
            var cleanPath = decodeURIComponent(path);
            openFileSignal(cleanPath)
        }
        onRejected: {
            console.log("Canceled")
        }
        Component.onCompleted: visible = false
    }

    toolBar:ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 2
            ToolButton {
                id: updateBtn
                height: parent.height
                width: parent.height
                Image {
                    source: "qrc:/icons/update.svg"
                    anchors.fill: parent
                    anchors.margins: 4
                }
                onClicked: {
                    console.log("on update all")
                    rootWindow.updateAllSignal("qml")
                }
            }

            ToolButton {
                height: parent.height
                width: parent.height
                anchors.left: updateBtn.right

                Image {
                    source: "qrc:/icons/open.svg"
                    anchors.fill: parent
                    anchors.margins: 4
                }
                onClicked: {
                    console.log("open log")
                    openFileDialog.open()

                }
            }

            //            ToolButton {
            //                height: parent.height
            //                width: parent.height
            //                anchors.left: updateBtn.right

            //                Image {
            //                    source: "qrc:/icons/merge.svg"
            //                    anchors.fill: parent
            //                    anchors.margins: 4
            //                }
            //                onClicked: {
            //                    console.log("show merge window")
            //                    var component = Qt.createComponent("qrc:/MergeWindow.qml");
            //                    var mergeWindow = component.createObject(rootWindow, {logsMap:logsMap});
            //                    mergeWindow.show()
            //                }
            //            }
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

    property var logsMap: ({})

    function addTab(tabTitle, model){
        console.log("add tab", tabTitle, "model", model)
        var tab=tabView.addTab(tabTitle,tabTemplate)
        tab.active=true
        logsMap[tabTitle]=model
        var obj=viewTemplate.createObject(tab.item,{logModel:model,rootWindow:rootWindow})
    }

    TabView{
        id: tabView
        anchors.fill: parent
        style: TabViewStyle {
            frameOverlap: 1
            tab: Rectangle {
                color: styleData.selected ? "steelblue" :"lightsteelblue"
                border.color:  "steelblue"
                implicitWidth: Math.max(text.width + 4+img.width, 80)
                implicitHeight: 20
                radius: 2
                Row{

                    Text {
                        id: text
                        text: styleData.title
                        color: styleData.selected ? "white" : "black"
                    }
                    Image{
                        id: img
                        width: 10; height:  10
                        source: "qrc:/icons/close.svg"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                console.log("clin on: close ",styleData.title )
                                tabView.removeTab(styleData.index)
                                var model=logsMap[styleData.title]
                                closeFileSignal(model.filename)
                            }
                        }
                    }
                }
            }
            //frame: Rectangle { color: "steelblue" }
        }
    }
}
