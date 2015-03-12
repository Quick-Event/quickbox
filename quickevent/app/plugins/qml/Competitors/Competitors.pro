message(including plugin $$PWD)

PLUGIN_NAME = Competitors

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

INCLUDEPATH += \

LIBS +=

include (src/src.pri)

RESOURCES += \
#    $${PLUGIN_NAME}.qrc \

