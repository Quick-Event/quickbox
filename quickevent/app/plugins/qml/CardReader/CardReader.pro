message(including plugin $$PWD)

PLUGIN_NAME = CardReader

include ( ../quickeventqmlplugin.pri )

QT += widgets serialport sql

CONFIG += c++11 hide_symbols

INCLUDEPATH += $$PWD/../../../../../libsiut/include
LIBS += -lsiut

INCLUDEPATH += $$PWD/../Event/include
LIBS += \
    -L$$DESTDIR \
    -lEventplugin \

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_NAME}.qrc \

