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

    property var selectedTextEdit: null
    function addHighlightedText(str){
        if(str!==null && str!==""){
            console.log("addHighlightedText: ",str)
            addHighlightedTextSignal(str);
        }
    }

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
        height: 30
        Row {

            anchors.fill: parent
            spacing: 2
            Button {
                id: updateBtn
                height: parent.height
                width: parent.height
                Image {
                    source: "qrc:/icons/update.svg"
                    anchors.fill: parent
                }
                onClicked: {
                    console.log("on update all")
                    rootWindow.updateAllSignal("qml")
                }
            }

            Button {
                height: parent.height
                width: parent.height

                Image {
                    source: "qrc:/icons/open.svg"
                    anchors.fill: parent
                }
                onClicked: {
                    console.log("open log")
                    openFileDialog.open()

                }
            }

            Button {
                tooltip: qsTr("addHighlightedText")
                height: parent.height
                width: parent.height
                Image {
                    source: "qrc:/icons/felt.svg"
                    anchors.fill: parent
                }

                onClicked: {
                    if(selectedTextEdit!==null){
                        addHighlightedText(selectedTextEdit.selectedText)
                    }

                }
            }
            Button {
                tooltip: qsTr("clearHighlightedText")
                height: parent.height
                width: parent.height
                Image {
                    source: "qrc:/icons/clear.svg"
                    anchors.fill: parent
                }

                onClicked: {
                    console.log("clearHighlightedTextSignal: ")
                    clearHighlightedTextSignal()
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

    property var logsMap: ({})

    function addTab(tabTitle, model){
        console.log("add tab", tabTitle, "model", model)
        var tab=tabView.addTab(tabTitle,tabTemplate)
        tab.active=true
        logsMap[tabTitle]=model
        var obj=viewTemplate.createObject(tab.item,{logModel:model,rootWindow:rootWindow})
    }

    Item{
        anchors.fill: parent

        Keys.onPressed: {
            console.log("Keys.onPressed",event.key);
            if (event.key === Qt.Key_W && event.modifiers === Qt.ControlModifier) {
                console.log("close");
                event.accepted = true;
                var curTab=tabView.getTab(tabView.currentIndex)
                tabView.removeTab(tabView.currentIndex)
                var model=logsMap[curTab.title]
                closeFileSignal(model.filename)
            }
            if (event.key === Qt.Key_O && event.modifiers === Qt.ControlModifier) {
                console.log("open");
                event.accepted = true;
                openFileDialog.open()
            }
            if (event.key === Qt.Key_L && event.modifiers === Qt.ControlModifier) {
                console.log("highlight");
                event.accepted = true;
                addHighlightedText(selectedTextEdit.selectedText)
            }
        }


        TabView{
            id: tabView
            anchors.fill: parent
        }
    }
}
