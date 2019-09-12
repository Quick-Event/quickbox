message(including plugin $$PWD)

PLUGIN_NAME = Event

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

CONFIG += c++11 hide_symbols

INCLUDEPATH += \
	#$$QF_PROJECT_TOP_SRCDIR/libsiut/include \

DEFINES += EVENTPLUGIN_BUILD_DLL

include (src/src.pri)

RESOURCES += \
#    $${PLUGIN_NAME}.qrc \

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \
	$${PLUGIN_NAME}.pl_PL.ts \
	$${PLUGIN_NAME}.nb_NO.ts \
	$${PLUGIN_NAME}.ru_RU.ts \
	$${PLUGIN_NAME}.nl_BE.ts \

lupdate_only {
SOURCES += \
	$$PWD/qml/*.qml \
}
