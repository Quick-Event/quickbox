message(including $$PWD)

TEMPLATE = lib
CONFIG += plugin
QT += qml widgets

unix:DESTDIR = $$OUT_PWD/../../lib/qml/qf/qmlwidgets
win:DESTDIR = $$OUT_PWD/../../bin/qml/qf/qmlwidgets
TARGET  = qfqmlwidgets

unix {
	qmlfiles.commands = ln -sf $$PWD/qml/qf/qmlwidgets/* $$DESTDIR
}
win {
	qmlfiles.commands = cp -rf $$PWD/qml/qf/qmlwidgets/* $$DESTDIR
}

QMAKE_EXTRA_TARGETS += qmlfiles
POST_TARGETDEPS += qmlfiles

include (src/src.pri)
