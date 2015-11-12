TEMPLATE = lib
TARGET = quickevent

QT += widgets sql qml multimedia

CONFIG += qt dll
CONFIG += c++11 hide_symbols

DEFINES += QUICKEVENT_BUILD_DLL

unix:DESTDIR = $$OUT_PWD/../../lib
win32:DESTDIR = $$OUT_PWD/../../bin

message(Target: $$TARGET)

LIBS += -L$$DESTDIR -lqfcore -lqfqmlwidgets

INCLUDEPATH += \
	$$PWD/include \
	$$PWD/../../libqf/libqfcore/include \
	$$PWD/../../libqf/libqfqmlwidgets/include \

message(INCLUDEPATH: $$INCLUDEPATH)

include ( src/src.pri )

RESOURCES += lib.qrc

