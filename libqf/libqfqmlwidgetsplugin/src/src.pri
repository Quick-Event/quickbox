message(including $$PWD)

INCLUDEPATH += $$PWD/../../libqfcore/include
INCLUDEPATH += $$PWD/../../libqfqmlwidgets/include

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/qmlwidgetssingleton.cpp \
    $$PWD/messageboxsingleton.cpp \
    $$PWD/inputdialogsingleton.cpp

HEADERS += \
    $$PWD/qmlwidgetssingleton.h \
    $$PWD/messageboxsingleton.h \
    $$PWD/inputdialogsingleton.h
