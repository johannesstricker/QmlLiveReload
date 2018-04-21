import QtQuick 2.0
import QtQuick.Window 2.2

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Loader {
        id: _loader
        objectName: "loader"
        function reload(path) {
            source = "";
            $QmlEngine.clearCache();
            source = path;
        }
        anchors.fill: parent
        anchors.centerIn: parent
        source: ""
    }
}
