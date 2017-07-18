import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.3

Item {
    id: viewerRoot
    property var rootWindow:null
    property var logModel:null
    property int fontSize: 10
    property string fontFace: "Monospace"
    property var editorFont: fontSize + "pt \"" + fontFace + "\"" + ", monospace"

    onLogModelChanged: {
        console.log("onLogModelChanged");
        if(logModel!=null){
            logModel.setQmlObject(viewerRoot)
        }
    }

    FontMetrics {
        id: fontMetrics
        font.family: viewerRoot.fontFace
        font.pointSize: viewerRoot.fontSize
    }

    property var lineHeight: fontMetrics.lineSpacing

    function scrollDown(){
        console.log("scroll down");
        scrollId.flickableItem.contentY=scrollId.flickableItem.contentHeight - tableView.height;
    }

    Component {
        id: contactDelegate

        Rectangle {
            id: wrapper
            height: lineHeight
            radius: 5
            width: parent.width
            TextEdit {
                id: te
                width: parent.width
                text: modelData
                activeFocusOnPress: true
                selectByMouse: true
                textFormat: TextEdit.RichText
                persistentSelection:false
                onActiveFocusChanged: {
                    if(activeFocus){
                        rootWindow.selectedTextEdit=te
                        tableView.currentIndex = index
                        te.select(0,0)
                    }
                    font.italic = activeFocus

                }
            }
        }
    }

    Column{
        anchors.fill: parent

        ToolBar {
            id: logTlbr
            height: 30
            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: false
            Layout.fillWidth: true
            Row {

                anchors.fill: parent
                spacing: 2

                Button {
                    tooltip: qsTr("addHighlightedText")
                    height: parent.height
                    width: parent.height

                    Image {
                        source: "qrc:/icons/felt.svg"
                        anchors.fill: parent
                        antialiasing: true
                    }

                    onClicked: {
                        if(rootWindow.selectedTextEdit!==null){
                            logModel.localHightlightPattern(rootWindow.selectedTextEdit.selectedText)
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
                        logModel.clearHightlight();
                    }
                }

                Button {
                    tooltip: qsTr("scroll down")
                    height: parent.height
                    width: parent.height
                    Image {
                        source: "qrc:/icons/down.svg"
                        anchors.fill: parent
                    }

                    onClicked: {
                        scrollDown();
                    }
                }
            }
        }

        ScrollView{
            id: scrollId
            width: parent.width
            height: parent.height - logTlbr.height
            ListView {
                id: tableView
                anchors.fill: parent
                width: parent.width
                focus: true

                model:logModel
                highlightFollowsCurrentItem: false
                orientation: Qt.Vertical
                interactive: false
                delegate: contactDelegate
            }
        }
    }
}


