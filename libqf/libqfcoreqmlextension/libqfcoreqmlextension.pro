message(including $$PWD)

TEMPLATE = lib
CONFIG += plugin
QT += qml

unix:DESTDIR = $$OUT_PWD/../../lib/qml/qf/core
win:DESTDIR = $$OUT_PWD/../../bin/qml/qf/core
TARGET  = qfcoreqmlextension

LIBS +=      \
	-lqfcore  \

unix: LIBS +=  \
	-L$$OUT_PWD/../../lib  \

unix {
	qmlfiles.commands = ln -sf $$PWD/qml/qf/core/* $$DESTDIR
}
win {
	qmlfiles.commands = cp -rf $$PWD/qml/qf/core/* $$DESTDIR
}

QMAKE_EXTRA_TARGETS += qmlfiles
POST_TARGETDEPS += qmlfiles

#-------------------- obsolete ------------------------

#pluginfiles.files += \
#	$$PWD/qml/qf/core/qmldir \

#qml.files = plugins.qml
#qml.path += $$[QT_INSTALL_EXAMPLES]/qml/qmlextensionplugins
#target.path += $$[QT_INSTALL_EXAMPLES]/qml/qmlextensionplugins/imports/TimeExample
#pluginfiles.path += $$[QT_INSTALL_EXAMPLES]/qml/qmlextensionplugins/imports/TimeExample

#INSTALLS += target qml pluginfiles

OTHER_FILES += \
    qml/* \

include (src/src.pri)