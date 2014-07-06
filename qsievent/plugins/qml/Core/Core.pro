message(including $$PWD)

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
QT += qml widgets

PLUGIN_MODULE_NAME = Core

DESTDIR = $$OUT_PWD/../../../../bin/divers/qsievent/plugins/$$PLUGIN_MODULE_NAME
TARGET  = $${PLUGIN_MODULE_NAME}plugin

LIBS +=      \
	-lqfcore  \

unix: LIBS +=  \
	-L$$OUT_PWD/../../../../lib  \

include ($$PWD/../../../../crosscompile-support.pri)

unix {
	qmlfiles.commands = ln -sf $$PWD/qml/* $$DESTDIR
}
win {
	qmlfiles.commands = cp -rf $$PWD/qml/* $$DESTDIR
}

QMAKE_EXTRA_TARGETS += qmlfiles
POST_TARGETDEPS += qmlfiles

OTHER_FILES += \
    $$PWD/qml/* \

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_MODULE_NAME}.qrc \
