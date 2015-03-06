message(including plugin $$PWD)

PLUGIN_NAME = Event

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/libsiut/include

LIBS += -lsiut

include (src/src.pri)

RESOURCES += \
#    $${PLUGIN_NAME}.qrc \

