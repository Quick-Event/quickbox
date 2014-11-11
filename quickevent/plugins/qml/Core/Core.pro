message(including $$PWD)

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
QT += qml widgets

PLUGIN_MODULE_NAME = Core

DESTDIR = $$OUT_PWD/../../../../bin/divers/quickevent/plugins/$$PLUGIN_MODULE_NAME
TARGET  = $${PLUGIN_MODULE_NAME}plugin

LIBS += -lqfcore
unix: LIBS += -L$$OUT_PWD/../../../../lib
win32: LIBS += -L$$OUT_PWD/../../../../bin

include ($$PWD/../../../../crosscompile-support.pri)

unix {
	qmlfiles.commands = ln -sf $$PWD/qml/* $$DESTDIR
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
#    $${PLUGIN_MODULE_NAME}.qrc \
