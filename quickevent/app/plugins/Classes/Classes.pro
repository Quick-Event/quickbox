message(including plugin $$PWD)

PLUGIN_NAME = Classes

include ( ../quickeventplugin.pri )

QT += widgets sql xml

LIBS += \
    -lEventQEPlugin \

include (src/src.pri)

RESOURCES += \ \
    Classes.qrc

TRANSLATIONS += \
	$${PLUGIN_NAME}.cs_CZ.ts \
	$${PLUGIN_NAME}.pl_PL.ts \
	$${PLUGIN_NAME}.nb_NO.ts \
	$${PLUGIN_NAME}.ru_RU.ts \
	$${PLUGIN_NAME}.nl_BE.ts \

lupdate_only {
SOURCES += \
	$$PWD/qml/*.qml \
	$$PWD/qml/reports/*.qml \
}
