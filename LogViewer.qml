import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.3

Item {
    id: viewerRoot
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
            Text { text: modelData
                MouseArea {
                    anchors.fill: parent
                    onClicked: tableView.currentIndex = index
                }
            }
        }
    }

    ScrollView{
        anchors.fill: parent
        ListView {
            id: tableView
            anchors.fill: parent
            width: parent.width
            focus: true
            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: true
            Layout.fillWidth: true

            model:logModel
            highlightFollowsCurrentItem: false
            orientation: Qt.Vertical

            delegate: contactDelegate
            highlight: Rectangle {
                width: tableView.width;  height: lineHeight
                color: "#FFFF88"
                y: tableView.currentItem.y;
            }

        }
    }

}


