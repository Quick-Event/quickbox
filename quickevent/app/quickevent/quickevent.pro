message(including $$PWD)

QT += core gui qml widgets sql xml sql printsupport serialport

TEMPLATE = app

CONFIG += c++14 hide_symbols lrelease

PROJECT_TOP_SRCDIR = $$PWD
QF_PROJECT_TOP_SRCDIR = $$PWD/../../../
QF_PROJECT_TOP_BUILDDIR = $$shadowed($$QF_PROJECT_TOP_SRCDIR)

win32: LIB_DIR_NAME = bin
else:  LIB_DIR_NAME = lib
LIBS_DIR = $$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
TARGET = quickevent

INCLUDEPATH += \
    $$QF_PROJECT_TOP_SRCDIR/3rdparty/necrolog/include \
    $$QF_PROJECT_TOP_SRCDIR/libqf/libqfcore/include \
    $$QF_PROJECT_TOP_SRCDIR/libqf/libqfqmlwidgets/include \
    $$QF_PROJECT_TOP_SRCDIR/libquickevent/libquickeventcore/include \
    $$QF_PROJECT_TOP_SRCDIR/libquickevent/libquickeventgui/include \
    $$QF_PROJECT_TOP_SRCDIR/libsiut/include \

message(INCLUDEPATH: $$INCLUDEPATH)

include(plugins/shared/shared.pri)
include(plugins/Core/Core.pri)
include(plugins/Event/Event.pri)
include(plugins/Classes/Classes.pri)
include(plugins/Competitors/Competitors.pri)
include(plugins/Runs/Runs.pri)
include(plugins/Oris/Oris.pri)
include(plugins/CardReader/CardReader.pri)
include(plugins/Receipts/Receipts.pri)
include(plugins/Relays/Relays.pri)
include(plugins/Speaker/Speaker.pri)


LIBS += \
    -L$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME \

LIBS +=      \
    -lnecrolog  \
    -lqfcore  \
    -lqfqmlwidgets  \
    -lsiut \
    -lquickeventcore \
    -lquickeventgui \

unix: LIBS +=  \
	-L../../../lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib:\$\$ORIGIN/../lib/qml/quickevent\'  \

# exception backtrace support
CONFIG(debug, debug|release): unix: QMAKE_LFLAGS += -rdynamic

RESOURCES += \
    $$PWD/images/images.qrc \

RC_FILE = $$PWD/app.rc

include ($$QF_PROJECT_TOP_SRCDIR/appdatafiles.pri)

include($$PWD/src/src.pri)

QML_IMPORT_PATH += \
    $$PWD/plugins \
    $$QF_PROJECT_TOP_BUILDDIR/lib/qml \
    $$QF_PROJECT_TOP_BUILDDIR/lib/qml/quickevent \


win32:CONFIG(debug, debug|release):CONFIG += console
#CONFIG += console

TRANSLATIONS += \
	$${TARGET}.cs_CZ.ts \
	$${TARGET}.fr_FR.ts \
	$${TARGET}.nb_NO.ts \
	$${TARGET}.nl_BE.ts \
	$${TARGET}.pl_PL.ts \
	$${TARGET}.ru_RU.ts \
	$${TARGET}.uk_UA.ts \

# register copy commands as build targets
first.depends = $(first) shared Event Classes Competitors Runs CardReader Receipts Relays Speaker # Core Oris
export(first.depends)
QMAKE_EXTRA_TARGETS += first shared Event Classes Competitors Runs CardReader Receipts Relays Speaker # Core Oris
