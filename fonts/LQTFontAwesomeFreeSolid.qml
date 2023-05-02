import QtQuick 2.15

Item {
    property alias iconColor: innerText.color
    property alias iconUtf8: innerText.text
    property alias innerTextElement: innerText

    Text {
        id: innerText
        font.family: fontAwesomeFreeSolid.family
        font.styleName: fontAwesomeFreeSolid.styleName
        font.weight: fontAwesomeFreeSolid.weight
        font.pixelSize: height
        anchors.fill: parent
    }
}
