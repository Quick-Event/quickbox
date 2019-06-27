message(including plugin $$PWD)

TEMPLATE = aux
CONFIG += quickevent_pure_qml_plugin

PLUGIN_NAME = ExamplePureQml

include ( ../quickeventqmlplugin.pri )

OTHER_FILES += \
	*.qml \

lupdate_only {
SOURCES += \
	*.qml \
}

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \
    $${PLUGIN_NAME}.nb_NO.ts \
    $${PLUGIN_NAME}.ru_RU.ts \
