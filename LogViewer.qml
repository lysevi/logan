import QtQuick 2.5
import QtQuick.Controls 1.2

Item {
    id: viewerRoot
    property alias logModel: tableView.model
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
                border.width: 1
                border.color: "grey"
                MouseArea{
                    anchors.fill: parent

                    Canvas{
                        id: lineCanvas
                        anchors.fill: parent
                        //height: lineHeight + 2
                        onPaint: {
                            var ctx = lineCanvas.getContext('2d');
                            //console.log("modelData[i]")
                            var x=10
                            var y=lineHeight*0.75
                            ctx.font=editorFont;
                            for(var i=0;i<modelData.count;++i){
                                console.log(modelData.messages[i].shortMessage,x, y)
                                ctx.fillText(modelData.messages[i].shortMessage, x, y);

                                y=y+lineHeight
                            }
                        }
                    }
                    onClicked: {
                        //lineCanvas.update();
                        console.log("click on ", modelData.message)
                    }
                }
            }
        }
    }

}

