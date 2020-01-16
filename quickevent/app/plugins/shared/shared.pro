message(including plugin $$PWD)

TEMPLATE = aux
#CONFIG += quickevent_pure_qml_plugin

PLUGIN_NAME = shared

include ( ../quickeventplugin.pri )

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \
	$${PLUGIN_NAME}.pl_PL.ts \
	$${PLUGIN_NAME}.nb_NO.ts \
	$${PLUGIN_NAME}.ru_RU.ts \
	$${PLUGIN_NAME}.nl_BE.ts \

lupdate_only {
SOURCES += \
    $$PWD/qml/QuickEvent/reports/*.qml \
}
