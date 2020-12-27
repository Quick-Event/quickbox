message(including plugin $$PWD)

PLUGIN_NAME = Core

include ( ../quickeventplugin.pri )

QT += widgets sql

# CONFIG += c++11 hide_symbols

include (src/src.pri)

RESOURCES += \

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \
	$${PLUGIN_NAME}.fr_FR.ts \
	$${PLUGIN_NAME}.nb_NO.ts \
	$${PLUGIN_NAME}.nl_BE.ts \
	$${PLUGIN_NAME}.pl_PL.ts \
	$${PLUGIN_NAME}.ru_RU.ts \
	$${PLUGIN_NAME}.uk_UA.ts \

# lupdate_only {
# SOURCES += \
# 	$$PWD/qml/*.qml \
# }
