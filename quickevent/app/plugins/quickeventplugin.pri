PLUGIN_TOP_SRCDIR = $$PWD/$$PLUGIN_NAME
QF_PROJECT_TOP_SRCDIR = $$PWD/../../..
QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../../../..

#PLUGIN_NAMESPACE_PATH = quickevent

include ( $$QF_PROJECT_TOP_SRCDIR/quickbox.pri )

TEMPLATE = lib
DESTDIR = $$LIBS_DIR

message (plugin $$PLUGIN_NAME)
message (INCLUDEPATH $$INCLUDEPATH)
message (QF_PROJECT_TOP_SRCDIR $$QF_PROJECT_TOP_SRCDIR)
message (QF_PROJECT_TOP_BUILDDIR $$QF_PROJECT_TOP_BUILDDIR)

#QT += qml
PLUGIN_NAME_UPPER = $$upper($$PLUGIN_NAME)
DEFINES += $${PLUGIN_NAME_UPPER}PLUGIN_BUILD_DLL
#message (l $$PLUGIN_NAME u $$PLUGIN_NAME_UPPER u2 $$upup)

TARGET = $${PLUGIN_NAME}QEPlugin

INCLUDEPATH += \
	$$QF_PROJECT_TOP_SRCDIR/libqf/libqfcore/include \
	$$QF_PROJECT_TOP_SRCDIR/libqf/libqfqmlwidgets/include \
	$$QF_PROJECT_TOP_SRCDIR/libquickevent/libquickeventcore/include \
	$$QF_PROJECT_TOP_SRCDIR/libquickevent/libquickeventgui/include \

LIBS += \
    -L$$LIBS_DIR \

unix: LIBS +=  \
	-Wl,-rpath,\'\$\$ORIGIN\'  \


LIBS += \
	-lqfcore \
	-lqfqmlwidgets \
	-lquickeventcore \
	-lquickeventgui \



