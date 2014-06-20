import QtQml 2.0

QfObject {
    QtObject{}
    QtObject{}
    QtObject{}
    QtObject{}
    Component.onCompleted: {
        console.debug("child count:", children.length)
    }
}
