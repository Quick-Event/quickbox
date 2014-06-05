import QtQuick 2.2
import "./sievent" as SiEvent
import qf.core 1.0

QtObject {
    id: root
    property list<QtObject> extensions: [
        //QtObject {}
        SiEvent.CardReadOut { id: sieventCardReadOut }
    ]
    Component.onCompleted: {
        Log.info("installed", "extensions:")
        for(var i=0; i<extensions.length; i++) {
                Log.info("\t", extensions[i]);
        }
    }
}
