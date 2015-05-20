message(including plugin $$PWD)

PLUGIN_NAME = Recipes

include ( ../quickeventqmlplugin.pri )

QT += widgets sql printsupport

CONFIG += c++11 hide_symbols

DEFINES += RECIPESPLUGIN_BUILD_DLL

INCLUDEPATH += $$PWD/../Event/include
INCLUDEPATH += $$PWD/../Runs/include
INCLUDEPATH += $$PWD/../CardReader/include
LIBS += \
    -L$$DESTDIR \
    -lEventplugin \
    -lRunsplugin \
    -lCardReaderplugin \

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_NAME}.qrc \

DISTFILES += \
    qml/reports/recipeClassic.qml

