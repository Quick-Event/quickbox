message(including plugin $$PWD)

PLUGIN_NAME = Core

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

CONFIG += c++11 hide_symbols

include (src/src.pri)

RESOURCES += \

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \
    $${PLUGIN_NAME}.nb_NO.ts \
    $${PLUGIN_NAME}.ru_RU.ts \

lupdate_only {
SOURCES += \
	$$PWD/qml/*.qml \
}
