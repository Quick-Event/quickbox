import QtQuick 2.2
import qf.core 1.0

QtObject {
    property string name: 'untitled-extension'
    function install(extension_connector)
    {
        qf.core.Log.debug('installing extension', name);
    }
}
