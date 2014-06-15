import QtQuick 2.2
import "./sievent" as SiEvent
import qf.core 1.0

QfObject {
    SiEvent.CardReadOut { id: sieventCardReadOut }
    Component.onCompleted: {
        Log.info("installed", "extensions:")
        for(var i=0; i<children.length; i++) {
                Log.info("\t", children[i]);
        }
    }
}
