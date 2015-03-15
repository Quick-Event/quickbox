message(including plugin $$PWD)

PLUGIN_NAME = Classes

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

CONFIG += c++11 hide_symbols

INCLUDEPATH += $$PWD/../../../../lib/include
LIBS += -lquickevent

INCLUDEPATH += $$PWD/../Event/include
LIBS += \
    -L$$DESTDIR \
    -lEventplugin \

include (src/src.pri)

RESOURCES += \
#    $${PLUGIN_NAME}.qrc \

