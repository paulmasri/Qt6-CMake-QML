import QtQuick

Window {
    visible: true
    width: 800
    height: 600

    ListModel {
        id: discModel
    }

    Repeater {
        model: discModel

        delegate: Disc {
            startPosition: model.startPosition
            velocity: model.velocity
            size: model.size
            bounds: model.bounds
            startHue: model.startHue

            Timer {
                interval: 3000
                running: true
                onTriggered: {
                    discModel.remove(index)
                }
            }
        }
    }

    Timer {
        interval: 200
        repeat: true
        running: true
        onTriggered: {
            discModel.append({
                startPosition: mouseArea.containsMouse ?
                                   {x: mouseArea.mouseX, y: mouseArea.mouseY} :
                                   {x: Math.random() * 800, y: Math.random() * 600},
                velocity: {x: Math.random() * 400 - 100, y: Math.random() * 400 - 100},
                size: Math.random() * 50 + 50,
                bounds: {x: 0, y: 0, width: 800, height: 600},
                startHue: Math.random()
            })
        }
    }

    MouseArea { id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }
}
