message(including $$PWD)

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
QT += qml sql widgets

PLUGIN_MODULE_NAME = Core

DEFINES += QE_CORE_PLUGIN_BUILD_DLL

DESTDIR = $$OUT_PWD/../../../../bin/divers/quickevent/plugins/$$PLUGIN_MODULE_NAME
TARGET  = $${PLUGIN_MODULE_NAME}plugin

INCLUDEPATH += \
	$$PWD/../../../../libqf/libqfcore/include \
	$$PWD/../../../../libqf/libqfqmlwidgets/include \
	$$PWD/../../../../libqf/libqfcoreqmlplugin/include \

LIBS += \
	-lqfcore \
	-lqfcoreqmlplugin  \

unix: LIBS += \
	-L$$OUT_PWD/../../../../lib \
	-L$$OUT_PWD/../../../../lib/qml \

win32: LIBS +=  \
	-L$$OUT_PWD/../../../../bin \
	-L$$OUT_PWD/../../../../bin/qml \

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
