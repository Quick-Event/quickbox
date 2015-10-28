message(including plugin $$PWD)

PLUGIN_NAME = Runs

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

CONFIG += c++11 hide_symbols

DEFINES += RUNSPLUGIN_BUILD_DLL

INCLUDEPATH += \
    $$PWD/src \
    $$PWD/../Event/include \
#    $$PWD/../Competitors/include \

LIBS += \
    -L$$DESTDIR \
    -lEventplugin \
#    -lCompetitorsplugin \

include (src/src.pri)

RESOURCES += \
#    $${PLUGIN_NAME}.qrc \

OTHER_FILES += \
	$$PWD/qml/reports/* \

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \

lupdate_only {
SOURCES += \
	$$PWD/qml/*.qml \
	$$PWD/qml/reports/*.qml \
}
