message(including $$PWD)

PLUGIN_NAME = qmlwidgets

include ( ../qfqmlplugin.pri )

QT += qml widgets sql network

INCLUDEPATH += ../../libqfqmlwidgets/include

LIBS += -lqfqmlwidgets

include (src/src.pri)

RESOURCES += \
	#$$PLUGIN_NAME.qrc

