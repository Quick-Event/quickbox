message(including $$PWD)

PLUGIN_NAME = qmlreports

include ( ../qfqmlplugin.pri )

QT += qml sql network

INCLUDEPATH += ../../libqfqmlwidgets/include

LIBS += -lqfqmlwidgets

include (src/src.pri)

RESOURCES += \
	#$$PLUGIN_NAME.qrc

