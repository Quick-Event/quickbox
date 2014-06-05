DEFINES +=    \
	SIUT_BUILD_DLL

LIBS +=       \
	-L libqf/libqfcore
	-lqfcore   \

CONFIG += hide_symbols

INCLUDEPATH += \
	$$PWD/include \
	$$PWD/../libqf/libqfcore/include \

include($$PWD/src/src.pri)
