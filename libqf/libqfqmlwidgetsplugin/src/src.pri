message(including $$PWD)

INCLUDEPATH += $$PWD/../../libqfcore/include
INCLUDEPATH += $$PWD/../../libqfqmlwidgets/include

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/qmlwidgetssingleton.cpp

HEADERS += \
    $$PWD/qmlwidgetssingleton.h
