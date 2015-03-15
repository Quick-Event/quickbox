message(including plugin $$PWD)

PLUGIN_NAME = Event

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

CONFIG += c++11 hide_symbols

INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/libsiut/include

DEFINES += EVENTPLUGIN_BUILD_DLL

include (src/src.pri)

RESOURCES += \
#    $${PLUGIN_NAME}.qrc \

