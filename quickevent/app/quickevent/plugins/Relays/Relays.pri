message(including plugin $$PWD)

PLUGIN_NAME = Relays

include (src/src.pri)

OTHER_FILES += \
        $$PWD/qml/reports/* \

TRANSLATIONS += \
        $$PWD/$${PLUGIN_NAME}.cs_CZ.ts \
	$$PWD/$${PLUGIN_NAME}.fr_FR.ts \
	$$PWD/$${PLUGIN_NAME}.nb_NO.ts \
	$$PWD/$${PLUGIN_NAME}.nl_BE.ts \
	$$PWD/$${PLUGIN_NAME}.pl_PL.ts \
	$$PWD/$${PLUGIN_NAME}.ru_RU.ts \
	$$PWD/$${PLUGIN_NAME}.uk_UA.ts \

lupdate_only {
SOURCES += \
        $$PWD/qml/*.qml \
	$$PWD/qml/reports/*.qml \
}

SRC_DATA_DIR = $$PWD/qml/*
DEST_DATA_DIR = $$LIBS_DIR/qml/quickevent/$$PLUGIN_NAME

unix: $${PLUGIN_NAME}.commands = $(MKDIR) $$DEST_DATA_DIR; $(COPY_DIR) $$SRC_DATA_DIR $$DEST_DATA_DIR
win32: $${PLUGIN_NAME}.commands = $(COPY_DIR) \"$$shell_path($$SRC_DATA_DIR)\" \"$$shell_path($$DEST_DATA_DIR)\"
export($${PLUGIN_NAME}.commands)

