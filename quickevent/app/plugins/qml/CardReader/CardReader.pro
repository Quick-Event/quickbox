message(including plugin $$PWD)

PLUGIN_NAME = CardReader

include ( ../quickeventqmlplugin.pri )

QT += widgets serialport sql

CONFIG += c++11 hide_symbols

DEFINES += CARDREADERPLUGIN_BUILD_DLL

INCLUDEPATH += $$PWD/../../../../../libsiut/include

LIBS += -lsiut

INCLUDEPATH += $$PWD/../Event/include
INCLUDEPATH += $$PWD/../Runs/include

LIBS += \
    -L$$DESTDIR \
    -lEventplugin \
    -lRunsplugin \

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_NAME}.qrc \

OTHER_FILES += \
	$$PWD/qml/CardCheckers/* \

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \

lupdate_only {
SOURCES += \
	$$PWD/qml/*.qml \
	$$PWD/qml/CardCheckers/*.qml \
}
