PLUGIN_TOP_SRCDIR = $$PWD/$$PLUGIN_NAME
QF_PROJECT_TOP_SRCDIR = $$PWD/../../../..
QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../../../../..

PLUGIN_NAMESPACE_PATH = quickevent

include ( $$QF_PROJECT_TOP_SRCDIR/qmlplugin.pri )

QT += qml

INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/libqf/libqfcore/include
INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/libqf/libqfqmlwidgets/include
LIBS += -lqfcore -lqfqmlwidgets
LIBS += -L$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME

