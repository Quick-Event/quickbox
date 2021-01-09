PLUGIN_TOP_SRCDIR = $$PWD/$$PLUGIN_NAME

#PLUGIN_NAMESPACE_PATH = quickevent

include ( ../../../quickbox.pri )

TEMPLATE = lib
DESTDIR = $$LIBS_DIR

message (plugin $$PLUGIN_NAME)
message (INCLUDEPATH $$INCLUDEPATH)
message (QF_PROJECT_TOP_SRCDIR $$QF_PROJECT_TOP_SRCDIR)
message (QF_PROJECT_TOP_BUILDDIR $$QF_PROJECT_TOP_BUILDDIR)

QT += qml

PLUGIN_NAME_UPPER = $$upper($$PLUGIN_NAME)
DEFINES += $${PLUGIN_NAME_UPPER}PLUGIN_BUILD_DLL
#message (l $$PLUGIN_NAME u $$PLUGIN_NAME_UPPER u2 $$upup)

TARGET = $${PLUGIN_NAME}QEPlugin

INCLUDEPATH += \
    $$QF_PROJECT_TOP_SRCDIR/3rdparty/necrolog/include \
	$$QF_PROJECT_TOP_SRCDIR/libqf/libqfcore/include \
	$$QF_PROJECT_TOP_SRCDIR/libqf/libqfqmlwidgets/include \
	$$QF_PROJECT_TOP_SRCDIR/libquickevent/libquickeventcore/include \
	$$QF_PROJECT_TOP_SRCDIR/libquickevent/libquickeventgui/include \
    $$PWD/include

LIBS += \
    -L$$LIBS_DIR \

unix: LIBS +=  \
	-Wl,-rpath,\'\$\$ORIGIN\'  \


LIBS += \
    -lnecrolog \
	-lqfcore \
	-lqfqmlwidgets \
	-lquickeventcore \
	-lquickeventgui \

SRC_DATA_DIR = $$PLUGIN_TOP_SRCDIR/qml
DEST_DATA_DIR = $$DESTDIR/qml/quickevent/$$PLUGIN_NAME
include ( $$QF_PROJECT_TOP_SRCDIR/datafiles.pri )

