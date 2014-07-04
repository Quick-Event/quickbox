message("including $$PWD")

TEMPLATE = lib
TARGET_NAME=qfqmlwidgets
unix:TARGET = $$OUT_PWD/../../lib/$${TARGET_NAME}

QT += widgets qml

CONFIG += c++11

#QMAKE_CXXFLAGS += -std=c++1y

DEFINES += QFQMLWIDGETS_BUILD_DLL

LIBS +=      \
	-lqfcore  \

win32: LIBS +=  \
	-L../../bin  \

unix: LIBS +=  \
	-L../../lib  \

include($$PWD/src/src.pri)

RESOURCES += \
    $${TARGET_NAME}.qrc \
