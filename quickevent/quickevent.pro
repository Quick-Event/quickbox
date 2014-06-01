
QT += core gui qml widgets
CONFIG += C++11

TEMPLATE = app

TARGET = $$OUT_PWD/../bin/quickevent

INCLUDEPATH += $$PWD/../libqf/libqfcore/include

include($$PWD/src/src.pri)
