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
        Item {
            width: 180; height: 40
            Row {
                Text { text: '<b>' + modelData.timestamp +'</b>'  }
                Text { text: modelData.type+" " }
                Text { text: modelData.message }
            }
        }
    }



    ListView {
        id: tableView
        anchors.fill: parent
        width: parent.width
        focus: true
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth: true

        model:logModel.lines

        delegate: contactDelegate
    }
}




