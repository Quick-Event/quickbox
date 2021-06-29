message(including plugin $$PWD)

PLUGIN_NAME = Oris

include (src/src.pri)

OTHER_FILES += \
        $$PWD/qml/reports/* \

lupdate_only {
SOURCES += \
        $$PWD/qml/*.qml \
	$$PWD/qml/reports/*.qml \
}

copyFiles($$SRC_DATA_DIR, $$DEST_DATA_DIR)
