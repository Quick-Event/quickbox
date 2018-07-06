message(including plugin $$PWD)

PLUGIN_NAME = Competitors

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

CONFIG += c++11 hide_symbols

DEFINES += COMPETITORSPLUGIN_BUILD_DLL

INCLUDEPATH += \
    $$PWD/../../../../../libquickevent/libquickeventcore/include \
    $$PWD/../../../../../libquickevent/libquickeventgui/include \
    $$PWD/../Event/include \

LIBS += -lquickeventcore -lquickeventgui

LIBS += \
    -L$$DESTDIR \
    -lEventplugin \


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
