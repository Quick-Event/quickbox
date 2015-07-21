message(including plugin $$PWD)

TEMPLATE = aux
CONFIG += quickevent_pure_qml_plugin

PLUGIN_NAME = shared

include ( ../quickeventqmlplugin.pri )

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \

lupdate_only {
SOURCES += \
	$$PWD/QuickEvent/reports/*.qml \
}
