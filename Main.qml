import QtQuick

Window {
    visible: true
    width: 800
    height: 600

    Text {
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 40
        text: mouseArea.containsMouse ? "Hello, World. I can see the mouse." : "Goodbye, World. I cannot see the mouse."
        color: mouseArea.containsMouse ? "green" : "red"
    }

    MouseArea { id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }
}
