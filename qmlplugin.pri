TEMPLATE = lib

CONFIG += plugin
CONFIG += c++11
CONFIG += hide_symbols

win32: LIB_DIR_NAME = bin
else:  LIB_DIR_NAME = lib

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME/qml/$$PLUGIN_NAMESPACE_PATH
TARGET  = $${PLUGIN_NAME}plugin

include ($$QF_PROJECT_TOP_SRCDIR/crosscompile-support.pri)

unix {
	# T flag is important, qml symlink in SRC/qml dir is created on second install without it
	qmlfiles.commands = \
		ln -sfT $$PLUGIN_TOP_SRCDIR/qml $$DESTDIR/$$PLUGIN_NAME
}
win32 {
	qmlfiles.commands = \
#		mkdir not needed for windows
		xcopy $$shell_path($$PLUGIN_TOP_SRCDIR/qml) $$shell_path($$DESTDIR/$$PLUGIN_NAME) /E /Y /I
}

QMAKE_EXTRA_TARGETS += qmlfiles
POST_TARGETDEPS += qmlfiles

OTHER_FILES += \
	$$PLUGIN_TOP_SRCDIR/qml/* \

QML_IMPORT_PATH += \
	$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME/qml \

message( PLUGIN_TOP_SRCDIR of $$PLUGIN_NAME: $$PLUGIN_TOP_SRCDIR )
message( PROJECT_TOPDIR: $$QF_PROJECT_TOP_SRCDIR )
message( PROJECT_TOP_BUILDDIR: $$QF_PROJECT_TOP_BUILDDIR )
message( OTHER_FILES: $$OTHER_FILES )
message( QML_IMPORT_PATH: $$QML_IMPORT_PATH )
message( DESTDIR: $$DESTDIR )
message( TARGET: $$TARGET )

