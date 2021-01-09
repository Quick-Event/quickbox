PLUGIN_TOP_SRCDIR = $$PWD/$$PLUGIN_NAME
QF_PROJECT_TOP_SRCDIR = $$PWD/../..
QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../../..

PLUGIN_NAMESPACE_PATH = qf

include ( ../../qmlplugin.pri )

INCLUDEPATH += ../../../3rdparty/necrolog/include
INCLUDEPATH += ../../libqfcore/include
LIBS += -lnecrolog
LIBS += -lqfcore
LIBS += -L$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME

