
QT += core gui qml widgets
CONFIG += C++11

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = quickevent
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \	
    frame.cpp \
    label.cpp

HEADERS  += mainwindow.h \
    frame.h \
    label.h

FORMS += \
	mainwindow.ui

OTHER_FILES += \
    main.qml
