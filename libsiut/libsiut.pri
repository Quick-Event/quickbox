DEFINES +=    \
	SIUT_BUILD_DLL

CONFIG += hide_symbols

INCLUDEPATH += \
    $$PWD/include \
	INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/3rdparty/necrolog/include \
	$$PWD/../libqf/libqfcore/include \

LIBS += -lnecrolog
LIBS += -lqfcore
LIBS += -L$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME

include($$PWD/src/src.pri)
