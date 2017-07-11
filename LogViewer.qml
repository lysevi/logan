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
    property var selectedTextEdit: null
    function addHighlightedText(str){
        if(str!==null && str!==""){
            console.log("addHighlightedText: ",str)
            rootWindow.addHighlightedTextSignal(str);
        }
    }
    Component {
        id: contactDelegate

        Rectangle {
            id: wrapper
            height: lineHeight
            radius: 5
            TextEdit {
                id: te
                text: modelData
                activeFocusOnPress: true
                //mouseSelectionMode:TextEdit.SelectWords
                selectByMouse: true
                textFormat: TextEdit.RichText
                //persistentSelection:true
                onActiveFocusChanged: {
                    if(activeFocus){
                        selectedTextEdit=te
                    }
                    font.italic = activeFocus
                    tableView.currentIndex = index
                }
            }
        }
    }

    ColumnLayout{
        anchors.fill: parent
        ToolBar{

            height: 20
            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: false
            Layout.fillWidth: true
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

        }

        ScrollView{
            Layout.alignment: Qt.AlignBottom
            Layout.fillHeight: true
            Layout.fillWidth: true
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
                highlight: Rectangle {
                    width: tableView.width;  height: lineHeight
                    color: "#FFFF88"
                    y: tableView.currentItem.y;
                }

            }
        }
    }
}


