message(including plugin $$PWD)

PLUGIN_MODULE_NAME = Event

include ( ../quickeventplugin.pri )

QT += serialport sql

INCLUDEPATH += $$PWD/../../../../../libsiut/include

LIBS += -lsiut

include (src/src.pri)

RESOURCES += \
#    $${PLUGIN_MODULE_NAME}.qrc \

