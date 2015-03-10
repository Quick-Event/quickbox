message(including $$PWD)

PLUGIN_NAME = qmlreports

include ( ../qfqmlplugin.pri )

QT += qml sql network

INCLUDEPATH += src
INCLUDEPATH += ../../libqfqmlwidgets/include
#INCLUDEPATH += $$PWD/../../libqfcore/include
#INCLUDEPATH += $$PWD/../../libqfqmlwidgets/include

LIBS += -lqfqmlwidgets

include (src/src.pri)

RESOURCES += \
	#$$PLUGIN_NAME.qrc

