import QtQuick 2.15

Item { id: root
    property point startPosition
    property point velocity
    property real size
    property rect bounds
    property real startHue

    Rectangle { id: disc
        property real hue: parent.startHue

        x: parent.startPosition.x - width / 2
        y: parent.startPosition.y - height / 2
        width: parent.size
        height: parent.size
        radius: width
        color: Qt.hsla(hue, 1, 0.5, 1)

        Timer {
            property double prevTimestamp: 0

            interval: 16
            repeat: true
            running: true
            onTriggered: {
                let period = 16
                let now = Date.now()
                if (prevTimestamp)
                    period = now - prevTimestamp
                prevTimestamp = now

                if (parent.x < root.bounds.x || parent.x > root.bounds.width - parent.width) {
                    root.velocity.x = -root.velocity.x
                    parent.x = Math.max(root.bounds.x, Math.min(root.bounds.width - parent.width, parent.x))
                }

                if (parent.y < root.bounds.y || parent.y > root.bounds.height - parent.height) {
                    root.velocity.y = -root.velocity.y
                    parent.y = Math.max(root.bounds.y, Math.min(root.bounds.height - parent.height, parent.y))
                }

                parent.x += root.velocity.x * period / 1000
                parent.y += root.velocity.y * period / 1000

                parent.hue = (parent.hue + period / 2000) % 1 // cycles every 2 seconds
            }
        }
    }
}
