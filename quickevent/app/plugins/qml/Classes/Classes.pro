message(including plugin $$PWD)

PLUGIN_NAME = Classes

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

CONFIG += c++11 hide_symbols

INCLUDEPATH +=

LIBS +=

include (src/src.pri)

RESOURCES += \
#    $${PLUGIN_NAME}.qrc \

