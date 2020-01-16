message(including plugin $$PWD)

PLUGIN_NAME = CardReader

include ( ../quickeventplugin.pri )

QT += widgets serialport sql

INCLUDEPATH += $$PWD/../../../../libsiut/include

LIBS += -lsiut

LIBS += \
    -lEventQEPlugin \
    -lRunsQEPlugin \

include (src/src.pri)

RESOURCES += \
     CardReader.qrc

OTHER_FILES += \
	$$PWD/qml/reports/* \

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
