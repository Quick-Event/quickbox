message(including plugin $$PWD)

PLUGIN_NAME = Runs

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

CONFIG += c++11 hide_symbols

DEFINES += RUNSPLUGIN_BUILD_DLL

INCLUDEPATH += \
    $$PWD/../Event/include \
    $$PWD/../Competitors/include \

LIBS += \
    -L$$DESTDIR \
    -lEventplugin \
    -lCompetitorsplugin \

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_NAME}.qrc \

OTHER_FILES += \
	$$PWD/qml/reports/* \

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \
	$${PLUGIN_NAME}.pl_PL.ts \
	$${PLUGIN_NAME}.nb_NO.ts \
	$${PLUGIN_NAME}.ru_RU.ts \
	$${PLUGIN_NAME}.nl_BE.ts \

lupdate_only {
SOURCES += \
	$$PWD/qml/*.qml \
	$$PWD/qml/reports/*.qml \
}
