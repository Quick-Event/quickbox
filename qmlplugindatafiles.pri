SRC_DATA_DIR_NAME = qml
SRC_DATA_DIR = $$PLUGIN_TOP_SRCDIR/$$SRC_DATA_DIR_NAME
DEST_DATA_DIR_NAME = $$PLUGIN_NAME
DEST_DATA_DIR = $$DESTDIR/$$DEST_DATA_DIR_NAME

include ($$PWD/datafiles.pri)
