HEADERS  +=  \
	$$PWD/siutglobal.h   \
    $$PWD/sicard.h \
    $$PWD/sipunch.h

include($$PWD/device/device.pri)
include($$PWD/message/message.pri)

SOURCES += \
    $$PWD/sicard.cpp \
    $$PWD/sipunch.cpp
