INCLUDEPATH += $$PWD/../../libqfcore/include

HEADERS += \
	$$PWD/qmlwidgetsglobal.h \
	$$PWD/frame.h \
	$$PWD/label.h \
    $$PWD/action.h \
    $$PWD/menu.h \
    $$PWD/menubar.h

SOURCES += \
	$$PWD/frame.cpp \
	$$PWD/label.cpp \
    $$PWD/action.cpp \
    $$PWD/menu.cpp \
    $$PWD/menubar.cpp

include ($$PWD/framework/framework.pri)
