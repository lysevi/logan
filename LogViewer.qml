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

    FontMetrics {
        id: fontMetrics
        font.family: viewerRoot.fontFace
        font.pointSize: viewerRoot.fontSize
    }

    property var lineHeight: fontMetrics.lineSpacing


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
                onActiveFocusChanged: {
                    if(activeFocus){
                        rootWindow.selectedTextEdit=te
                        tableView.currentIndex = index
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
                    }

                    onClicked: {
                        logModel.addHeighlightPattern(rootWindow.selectedTextEdit.selectedText)

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
                        logModel.clearHeightlight();
                    }
                }
            }
        }
        ScrollView{
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


