message(including plugin $$PWD)

PLUGIN_NAME = CardReadout

include ( ../quickeventqmlplugin.pri )

QT += widgets serialport sql

INCLUDEPATH += $$PWD/../../../../../libsiut/include

LIBS += -lsiut

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_NAME}.qrc \

