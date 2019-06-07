message(including plugin $$PWD)

PLUGIN_NAME = Oris

include ( ../quickeventqmlplugin.pri )

QT += widgets sql network

CONFIG += c++11 hide_symbols

INCLUDEPATH += \
    $$PWD/../Event/include \
    $$PWD/../Classes/include \
    $$PWD/../Competitors/include \

LIBS += \
    -L$$DESTDIR \
    -lEventplugin \
    -lClassesplugin \
    -lCompetitorsplugin \

include (src/src.pri)

#RESOURCES += \
#    $${PLUGIN_NAME}.qrc \

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \
    $${PLUGIN_NAME}.nb_NO.ts \

lupdate_only {
SOURCES += \
	$$PWD/qml/*.qml \
}
