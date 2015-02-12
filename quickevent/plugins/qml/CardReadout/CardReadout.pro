message(including $$PWD)

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
QT += qml widgets

PLUGIN_MODULE_NAME = CardReadout

win32: LIB_DIR_NAME = bin
else:  LIB_DIR_NAME = lib

DESTDIR = $$OUT_PWD/../../../../$$LIB_DIR_NAME
TARGET  = QE$${PLUGIN_MODULE_NAME}Plugin

INCLUDEPATH += $$PWD/../../../../libqf/libqfcore/include
INCLUDEPATH += $$PWD/../../../../libqf/libqfqmlwidgets/include

message(INCLUDEPATH: $$INCLUDEPATH)

LIBS += -lqfcore -lqfqmlwidgets

LIBS += -L$$OUT_PWD/../../../../$$LIB_DIR_NAME

include ($$PWD/../../../../crosscompile-support.pri)

unix {
	qmlfiles.commands = \
		mkdir -p $$DESTDIR/qml/quickevent $$escape_expand(\n\t) \
		ln -sfT $$PWD/qml $$DESTDIR/qml/quickevent/$$PLUGIN_MODULE_NAME
}
win32 {
	qmlfiles.commands = xcopy $$shell_path($$PWD/qml) $$shell_path($$DESTDIR) /e/y
}

QMAKE_EXTRA_TARGETS += qmlfiles
POST_TARGETDEPS += qmlfiles

OTHER_FILES += \
    $$PWD/qml/* \

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_MODULE_NAME}.qrc \
