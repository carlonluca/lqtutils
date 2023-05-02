import QtQuick 2.15

Item {
    property alias iconColor: innerText.color
    property alias iconUtf8: innerText.text
    property alias innerTextElement: innerText

    Text {
        id: innerText
        font.family: fontAwesomeBrandsRegular.family
        font.styleName: fontAwesomeBrandsRegular.styleName
        font.weight: fontAwesomeBrandsRegular.weight
        font.pixelSize: height
        anchors.fill: parent
    }
}
