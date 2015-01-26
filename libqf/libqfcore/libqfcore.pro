message("including $$PWD")

TEMPLATE = lib
TARGET=qfcore

QF_PROJECT_BUILD_ROOT = $(QF_PROJECT_BUILD_ROOT)
#message ( QF_PROJECT_BUILD_ROOT: $$QF_PROJECT_BUILD_ROOT )
isEmpty(QF_PROJECT_BUILD_ROOT) {
	QF_PROJECT_BUILD_ROOT = $$OUT_PWD/../..
}

unix:DESTDIR = $$QF_PROJECT_BUILD_ROOT/lib
win32:DESTDIR = $$QF_PROJECT_BUILD_ROOT/bin

message ( DESTDIR: $$DESTDIR )

QT += xml sql
CONFIG += C++11
CONFIG += hide_symbols

DEFINES += QFCORE_BUILD_DLL

include($$PWD/src/src.pri)

include ($$PWD/../../crosscompile-support.pri)

RESOURCES += \
    $${TARGET}.qrc \
