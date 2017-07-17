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
    title: qsTr("LView: ")+lview_version
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

    Timer {
        id: updateTimer
        interval: timerSlider.value*1000;
        running: timerEnableCheckbox.checked;
        repeat: true
        onTriggered: {
            console.log("update timer");
            updateAllSignal("null")
        }
    }

    toolBar:ToolBar {
        height: 50
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
                    rootWindow.updateAllSignal("null")
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


            CheckBox{
                id: timerEnableCheckbox
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Update timeout")
                checked: false
                onCheckedChanged: {
                    timerSlider.enabled=timerEnableCheckbox.checked
                }
            }

            Text {
                id: timerText
                text: timerSlider.value + " sec."
                width: 40
                anchors.verticalCenter: parent.verticalCenter
            }

            Slider{
                id: timerSlider
                anchors.verticalCenter: parent.verticalCenter
                minimumValue: 1
                maximumValue: 60
                enabled:false
                stepSize:1
                style: SliderStyle {
                    groove: Rectangle {
                        implicitWidth: 200
                        implicitHeight: 8
                        color:  timerSlider.enabled? "lightblue" : "gray"
                        radius: 8
                    }
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

            if (event.key === Qt.Key_F2) {
                console.log("update current");
                event.accepted = true;
                var curTab=tabView.getTab(tabView.currentIndex)
                updateAllSignal(curTab.title);
            }
        }


        TabView{
            id: tabView
            anchors.fill: parent
        }
    }
}
