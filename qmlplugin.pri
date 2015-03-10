TEMPLATE = lib

CONFIG += plugin
CONFIG += c++11
CONFIG += hide_symbols

win32: LIB_DIR_NAME = bin
else:  LIB_DIR_NAME = lib

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME/qml/$$PLUGIN_NAMESPACE_PATH
TARGET  = $${PLUGIN_NAME}plugin

message( PLUGIN_TOP_SRCDIR of $$PLUGIN_NAME: $$PLUGIN_TOP_SRCDIR )
message( PROJECT_TOPDIR: $$QF_PROJECT_TOP_SRCDIR )
message( PROJECT_TOP_BUILDDIR: $$QF_PROJECT_TOP_BUILDDIR )
message( DESTDIR: $$DESTDIR )
message( TARGET: $$TARGET )

include ($$QF_PROJECT_TOP_SRCDIR/crosscompile-support.pri)

unix {
	# T flag is important, qml symlink in SRC/qml dir is created on second install without it
	qmlfiles.commands = \
		ln -sfT $$PLUGIN_TOP_SRCDIR/qml $$DESTDIR/$$PLUGIN_NAME
}
win32 {
# TODO: implement Unix part also for Windows
#	qmlfiles.commands = \
#		mkdir -p $$DESTDIR/$$PLUGIN_MODULE_PATH $$escape_expand(\n\t) \
#		xcopy $$shell_path($$PWD/$$PLUGIN_MODULE_PATH/*) $$shell_path($$DESTDIR/$$PLUGIN_MODULE_PATH) /e/y
}

QMAKE_EXTRA_TARGETS += qmlfiles
POST_TARGETDEPS += qmlfiles

OTHER_FILES += \
	$$PLUGIN_TOP_SRCDIR/qml \

QML_IMPORT_PATH += \
	$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME/qml \

