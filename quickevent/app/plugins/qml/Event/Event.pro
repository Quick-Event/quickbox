message(including plugin $$PWD)

PLUGIN_NAME = Event

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/libsiut/include

LIBS += -lsiut

DEFINES += QE_EVENT_PLUGIN_BUILD_DLL

include (src/src.pri)

RESOURCES += \
#    $${PLUGIN_NAME}.qrc \

