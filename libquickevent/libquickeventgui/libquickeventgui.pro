TEMPLATE = lib
TARGET = quickeventgui

QT += widgets sql qml multimedia

CONFIG += qt dll
CONFIG += c++11 hide_symbols

DEFINES += QUICKEVENTGUI_BUILD_DLL

unix:DESTDIR = $$OUT_PWD/../../lib
win32:DESTDIR = $$OUT_PWD/../../bin

message(Target: $$TARGET)

LIBS += -L$$DESTDIR
LIBS += -lqfcore -lqfqmlwidgets
LIBS += -lsiut -lquickeventcore

INCLUDEPATH += \
	$$PWD/../../libqf/libqfcore/include \
	$$PWD/../../libqf/libqfqmlwidgets/include \
	$$PWD/../../libsiut/include \
	$$PWD/../../libquickevent/libquickeventcore/include

message(INCLUDEPATH: $$INCLUDEPATH)

include ( src/src.pri )

RESOURCES += libquickeventgui.qrc

