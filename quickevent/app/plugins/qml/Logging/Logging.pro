message(including plugin $$PWD)

PLUGIN_NAME = Logging

include ( ../quickeventqmlplugin.pri )

QT += widgets

CONFIG += c++11 hide_symbols

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_NAME}.qrc \

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \

lupdate_only {
SOURCES += \
	$$PWD/qml/*.qml \
}
