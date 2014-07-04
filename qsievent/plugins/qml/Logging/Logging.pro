message(including $$PWD)

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
QT += qml widgets

PLUGIN_MODULE_NAME = Logging

DESTDIR = $$OUT_PWD/../../../../bin/divers/qsievent/plugins/$$PLUGIN_MODULE_NAME
TARGET  = loggingplugin

LIBS +=      \
	-lqfcore  \

unix: LIBS +=  \
	-L$$OUT_PWD/../../../../lib  \

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
