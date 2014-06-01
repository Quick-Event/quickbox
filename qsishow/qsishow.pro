MY_SUBPROJECT = qsishow
!include(go_to_top.pri):error("cann't find go_to_top.pri")

message($$MY_SUBPROJECT)

TEMPLATE = app

QT += gui sql script scripttools #xml  scripttools

CONFIG +=                   \
	warn_on                   \
	qt                   \
	thread               \
#	uitools               \
#	assistant	            \

DEFINES += QT

#QML_IMPORT_PATH = /home/fanda/qt/si/qsishow/divers/qsishow/qml

# tohle zajisti, aby pri exception backtrace nasel symboly z aplikace
unix:QMAKE_LFLAGS_APP += -rdynamic

TARGET = $$MY_SUBPROJECT
DESTDIR = $$MY_BUILD_DIR/bin

INCLUDEPATH += $$MY_TOP_DIR/libqf/libqfcore/include 
#INCLUDEPATH += $$PWD/../libqfsiut/include

DOLAR=$

LIBS +=      \
#	-lqfsiut$$QF_LIBRARY_DEBUG_EXT  \
	-lqfcore$$QF_LIBRARY_DEBUG_EXT  \

win32: LIBS +=  \
	-L$$MY_BUILD_DIR/bin  \

unix: LIBS +=  \
	-L$$MY_BUILD_DIR/lib  \
	-Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\'  \

message(LIBS: $$LIBS)

#win32:CONFIG(debug, debug|release):
CONFIG += console
console: message(CONSOLE)

#RESOURCES    += $${MY_SUBPROJECT}.qrc
#FORMS    +=   \

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
	application.cpp \
    model.cpp \

HEADERS += application.h \
    model.h \

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
#qtcAddDeployment()

OTHER_FILES += \
    divers/qsishow/qml/Cell.qml \
    divers/qsishow/qml/scripts.js \
    divers/qsishow/qml/main.qml \
    divers/qsishow/qml/results/Components/qmldir \
    divers/qsishow/qml/results/Components/Detail.qml \
    divers/qsishow/qml/Components/qmldir \
    divers/qsishow/qml/Components/Detail.qml
