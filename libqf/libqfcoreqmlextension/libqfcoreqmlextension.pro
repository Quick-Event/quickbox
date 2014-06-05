message(including $$PWD)

TEMPLATE = lib
CONFIG += plugin
QT += qml

unix:DESTDIR = $$OUT_PWD/../../qml/qf/core
TARGET  = qfcoreqmlextension

SOURCES += plugin.cpp

INCLUDEPATH += $$PWD/../libqfcore/include

LIBS +=      \
	-lqfcore  \

unix: LIBS +=  \
	-L$$OUT_PWD/../../lib  \

pluginfiles.files += \
	$$PWD/qml/qf/core/qmldir \

#qmldir.target = $$DESTDIR/qmldir
qmldir.commands = mkdir -p $$DESTDIR && cp -rf $$PWD/qml/qf/core/* $$DESTDIR

QMAKE_EXTRA_TARGETS += qmldir
POST_TARGETDEPS += qmldir

#qml.files = plugins.qml
#qml.path += $$[QT_INSTALL_EXAMPLES]/qml/qmlextensionplugins
#target.path += $$[QT_INSTALL_EXAMPLES]/qml/qmlextensionplugins/imports/TimeExample
#pluginfiles.path += $$[QT_INSTALL_EXAMPLES]/qml/qmlextensionplugins/imports/TimeExample

#INSTALLS += target qml pluginfiles

OTHER_FILES += \
    qml/qf/core/Log.qml \
    qml/qf/core/qmldir
