import QtQuick 2.12
import QtQuick.Window 2.12
import com.luke 1.0

Window {
    property int enumValue: MySharedEnum.Value6
    property bool doBind: false

    visible: true

    Component.onCompleted: {
        console.log("Enum value:", enumValue)
        x = settings.appX
        y = settings.appY
        width = settings.appWidth
        height = settings.appHeight
        doBind = true
    }

    Connections {
        target: settings
        onAppWidthChanged: (w) => console.log("App width saved:", w)
        onAppHeightChanged: (h) => console.log("App height saved:", h)
    }

    Binding { when: doBind; target: settings; property: "appWidth"; value: width }
    Binding { when: doBind; target: settings; property: "appHeight"; value: height }
    Binding { when: doBind; target: settings; property: "appX"; value: x }
    Binding { when: doBind; target: settings; property: "appY"; value: y }
}
