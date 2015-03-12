message(including plugin $$PWD)

PLUGIN_NAME = Competitors

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

INCLUDEPATH += \
    $$PWD/../Event/include

LIBS += -L$$DESTDIR \
    -lEventplugin

include (src/src.pri)

RESOURCES += \
#    $${PLUGIN_NAME}.qrc \

