message(including plugin $$PWD)

PLUGIN_NAME = Receipts

include ( ../quickeventplugin.pri )

QT += widgets sql printsupport xml

#INCLUDEPATH += $$PWD/../../../../libsiut/include
LIBS += \
    -lEventQEPlugin \

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_NAME}.qrc \

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
	$$PWD/qml/reports/receipts/*.qml \
}
