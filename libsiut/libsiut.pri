DEFINES +=    \
	SIUT_BUILD_DLL

LIBS +=       \
	-L libqf/libqfcore
	-lqfcore   \

CONFIG += hide_symbols

INCLUDEPATH += \
	$$PWD/include \
	$$PWD/../libqf/libqfcore/include \

HEADERS += \
	$$PWD/include/commport.h    \
	$$PWD/include/simessagedata.h   \
	$$PWD/include/simessage.h   \
	$$PWD/include/sidevicedriver.h      \


include($$PWD/src/src.pri)
