import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.3

Item {
    id: viewerRoot
    property var logModel:null
    property int fontSize: 10
    property string fontFace: "Monospace"

    property var editorFont: fontSize + "pt \"" + fontFace + "\"" + ", monospace"

    Component.onCompleted: {
        if(logModel!==null){
            tableView.model=logModel.lines
        }
    }

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

    ColumnLayout {
        anchors.fill: parent
        spacing: 2

        TableView {
            id: tableView
            width: parent.width
            focus: true
            Layout.alignment: Qt.AlignTop
            Layout.fillHeight: true
            Layout.fillWidth: true

            onWidthChanged: {
                messageColumn.width=tableView.width/4*3
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

            TableViewColumn{
                role:"timestamp"
                title:"time"
            }

            TableViewColumn {
                id: messageColumn

                title: "message"
                width: tableView.width/4*3
                delegate: Rectangle{
                    id: canvasRect
                    height: lineHeight*modelData.count
                    border.width: 1
                    border.color: "lightgrey"

                    MouseArea{
                        anchors.fill: parent

                        Canvas{
                            id: lineCanvas
                            anchors.fill: parent

                            onWidthChanged: {
                                lineCanvas.requestPaint();
                            }

                            onHeightChanged: {
                                lineCanvas.requestPaint();
                            }

                            //height: lineHeight + 2
                            onPaint: {
                                var ctx = lineCanvas.getContext('2d');
                                ctx.clearRect(0,0, lineCanvas.width, lineCanvas.height)
                                ctx.renderStrategy =Canvas.Cooperative
                                //console.log("modelData[i]")
                                var x=10
                                var y=lineHeight*0.75
                                ctx.font=editorFont;
                                ctx.beginPath()
                                ctx.strokeStyle = 'black'
                                for(var i=0;i<modelData.count;++i){
                                    console.log(modelData.messages[i].shortMessage,x, y)
                                    ctx.strokeText(modelData.messages[i].shortMessage, x, y);

                                    y=y+lineHeight
                                }
                                ctx.stroke()
                            }
                        }

                        onClicked: {
                            console.log("click on ", styleData.row)
                            tableView.selection.clear()
                            tableView.selection.select(styleData.row)
                            lineCanvas.requestPaint()
                        }
                    }
                }


            }
        }


        Text {
            id: linesLebel
            Layout.alignment: Qt.AlignBottom
            text:qsTr("Lines:");
        }
        Text {
            Layout.alignment: Qt.AlignBottom
            anchors.left: linesLebel.right
            anchors.top: linesLebel.top
            text: {
                return logModel==null?0:logModel.count
            }
        }

    }
}

