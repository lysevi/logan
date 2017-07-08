import QtQuick 2.5
import QtQuick.Controls 1.2

Item {
    id: viewerRoot
    property alias logModel: tableView.model
    property int fontSize: 10
    property string fontFace: "Monospace"

    FontMetrics {
        id: fontMetrics
        font.family: viewerRoot.fontFace
        font.pointSize: viewerRoot.fontSize
    }

    property var lineHeight: fontMetrics.lineSpacing

    Component {
        id: logLineDelegate
        Item {
            anchors.fill: parent
            Text{
                anchors.fill: parent
                text: modelData
            }
        }
    }

    Component {
        id: rowDelegate
        Item{
            width: parent.width
            height: lineHeight*count
            Row {
                anchors.fill: parent
                spacing: 20

                Column {
                    Repeater {
                        model: message
                        delegate: logLineDelegate
                    }
                }
            }
        }

    }

    TableView {
        id: tableView
        anchors.fill: parent
        focus: true

        TableViewColumn{
            role:"timestamp"
            title:"time"
        }

        rowDelegate: Rectangle {
            height: lineHeight*modelData.count
            SystemPalette {
                id: myPalette;
                colorGroup: SystemPalette.Active
            }
            color: {
                var baseColor = styleData.alternate?myPalette.alternateBase:myPalette.base
                return styleData.selected?myPalette.highlight:baseColor
            }
        }

        TableViewColumn {
            id: messageColumn
            title: "message"
            width: tableView.width/4*3
            delegate: Rectangle{
                height: lineHeight*modelData.count
                border.width: 2
                border.color: "grey"
                Column {
                    anchors.fill: parent
                    Repeater {
                        model: modelData.message

                        delegate: Rectangle{
                            border.width: 0
                            color: ( index % 2 == 0 ) ? "white" : "lightblue"
                            height: lineHeight
                            width: messageColumn.width
                            Text {
                                text: modelData
                            }
                        }
                    }

                }
            }
        }
    }

}

