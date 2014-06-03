INCLUDEPATH += $$PWD/../../libqf/libqfcore/include
INCLUDEPATH += $$PWD/../../libqf/libqfqmlwidgets/include
#message(INCLUDEPATH: $$INCLUDEPATH)

SOURCES += \
	$$PWD/main.cpp\
	$$PWD/mainwindow.cpp

HEADERS  += \
	$$PWD/mainwindow.h

FORMS += \
	$$PWD/mainwindow.ui

OTHER_FILES += \
	$$PWD/main.qml
