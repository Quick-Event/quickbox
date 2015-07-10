PLUGIN_TOP_SRCDIR = $$PWD/$$PLUGIN_NAME
QF_PROJECT_TOP_SRCDIR = $$PWD/../../../..
QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../../../../..

PLUGIN_NAMESPACE_PATH = quickevent

quickevent_pure_qml_plugin {

message ( PURE QML PLUGIN $$PLUGIN_TOP_SRCDIR )

win32: LIB_DIR_NAME = bin
else:  LIB_DIR_NAME = lib

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME/qml/$$PLUGIN_NAMESPACE_PATH

OTHER_FILES += \
	$$PLUGIN_TOP_SRCDIR/$$PLUGIN_NAME/*

QML_IMPORT_PATH += \
	$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME/$$PLUGIN_NAME

unix {
	# T flag is important, qml symlink in SRC/qml dir is created on second install without it
	qmlfiles.commands = \
		ln -sfT $$PLUGIN_TOP_SRCDIR $$DESTDIR/$$PLUGIN_NAME
		#rsync -r $$PLUGIN_TOP_SRCDIR/qml $$DESTDIR/$$PLUGIN_NAME
}
win32 {
	#mkdir not needed for windows
	qmlfiles.commands = \
		xcopy $$shell_path($$PLUGIN_TOP_SRCDIR) $$shell_path($$DESTDIR/$$PLUGIN_NAME) /E /Y /I
}

#qmlfiles.depends = qmlfiles_conf

QMAKE_EXTRA_TARGETS += qmlfiles
#QMAKE_EXTRA_TARGETS += qmlfiles_conf
PRE_TARGETDEPS += qmlfiles

}
else {

include ( $$QF_PROJECT_TOP_SRCDIR/qmlplugin.pri )

QT += qml

INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/libqf/libqfcore/include
INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/libqf/libqfqmlwidgets/include

LIBS += \
    -L$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME \

LIBS += -lqfcore -lqfqmlwidgets

INCLUDEPATH += \
    $$PWD/../../../lib/include \

LIBS += -lquickevent

}


