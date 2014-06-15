import QtQuick 2.0

QtObject {
    // workaround for QTBUG-15127
    id: root
    default property alias children: root.children_helper
    property list<QtObject> children_helper
}
