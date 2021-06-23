message (plugin $$PLUGIN_NAME)

RESOURCES += \
      $${PLUGIN_DIR}/$${PLUGIN_NAME}.qrc

OTHER_FILES += \
        $${PLUGIN_DIR}/qml/reports/* \

TRANSLATIONS += \
        $${PLUGIN_DIR}/$${PLUGIN_NAME}.cs_CZ.ts \
	$${PLUGIN_DIR}/$${PLUGIN_NAME}.fr_FR.ts \
	$${PLUGIN_DIR}/$${PLUGIN_NAME}.nb_NO.ts \
	$${PLUGIN_DIR}/$${PLUGIN_NAME}.nl_BE.ts \
	$${PLUGIN_DIR}/$${PLUGIN_NAME}.pl_PL.ts \
	$${PLUGIN_DIR}/$${PLUGIN_NAME}.ru_RU.ts \
	$${PLUGIN_DIR}/$${PLUGIN_NAME}.uk_UA.ts \

lupdate_only {
SOURCES += \
        $${PLUGIN_DIR}/qml/*.qml \
	$${PLUGIN_DIR}/qml/reports/*.qml \
}

SRC_DATA_DIR = $${PLUGIN_DIR}/qml
DEST_DATA_DIR = $$LIBS_DIR/qml/quickevent/$$PLUGIN_NAME

unix: copydata$${PLUGIN_NAME}.commands = $(MKDIR) $$DEST_DATA_DIR; $(COPY_DIR) $$SRC_DATA_DIR $$DEST_DATA_DIR
win32: copydata$${PLUGIN_NAME}.commands = $(COPY_DIR) \"$$shell_path($$SRC_DATA_DIR)\" \"$$shell_path($$DEST_DATA_DIR)\"
first.depends = $(first) copydata$${PLUGIN_NAME}
export(first.depends)
export(copydata$${PLUGIN_NAME}.commands)
QMAKE_EXTRA_TARGETS += first copydata$${PLUGIN_NAME}

#exists( $$PWD/$${PLUGIN_NAME}/$${PLUGIN_NAME}.qrc ) {
#    RESOURCES += \
#            $$PWD/$${PLUGIN_NAME}/$${PLUGIN_NAME}.qrc
#}
#
#OTHER_FILES += \
#        $$PWD/$${PLUGIN_NAME}/qml/reports/* \
#
#TRANSLATIONS += \
#        $$PWD/$${PLUGIN_NAME}/$${PLUGIN_NAME}.cs_CZ.ts \
#	$$PWD/$${PLUGIN_NAME}/$${PLUGIN_NAME}.fr_FR.ts \
#	$$PWD/$${PLUGIN_NAME}/$${PLUGIN_NAME}.nb_NO.ts \
#	$$PWD/$${PLUGIN_NAME}/$${PLUGIN_NAME}.nl_BE.ts \
#	$$PWD/$${PLUGIN_NAME}/$${PLUGIN_NAME}.pl_PL.ts \
#	$$PWD/$${PLUGIN_NAME}/$${PLUGIN_NAME}.ru_RU.ts \
#	$$PWD/$${PLUGIN_NAME}/$${PLUGIN_NAME}.uk_UA.ts \
#
#lupdate_only {
#SOURCES += \
#        $$PWD/$${PLUGIN_NAME}/qml/*.qml \
#	$$PWD/$${PLUGIN_NAME}/qml/reports/*.qml \
#}
#
#SRC_DATA_DIR = $$PWD/$${PLUGIN_NAME}/qml
#DEST_DATA_DIR = $$LIBS_DIR/qml/quickevent/$$PLUGIN_NAME

#include(../../../../datafiles.pri)
#unix: copydata$${PLUGIN_NAME}.commands = mkdir -p $$DEST_DATA_DIR && $(COPY_DIR) \"$$SRC_DATA_DIR\" \"$$DEST_DATA_DIR\"
#win32: copydata$${PLUGIN_NAME}.commands = $(COPY_DIR) \"$$shell_path($$SRC_DATA_DIR)\" \"$$shell_path($$DEST_DATA_DIR)\"
#first.depends = $(first) copydata$${PLUGIN_NAME}
#export(first.depends)
#export(copydata$${PLUGIN_NAME}.commands)
#QMAKE_EXTRA_TARGETS += first copydata$${PLUGIN_NAME}
