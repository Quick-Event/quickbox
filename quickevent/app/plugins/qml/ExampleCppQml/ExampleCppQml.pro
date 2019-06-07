message(including plugin $$PWD)

PLUGIN_NAME = ExampleCppQml

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

CONFIG += c++11 hide_symbols

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
