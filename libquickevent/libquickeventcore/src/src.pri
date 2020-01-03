HEADERS  += \
    $$PWD/quickeventcoreglobal.h   \
    $$PWD/coursedef.h \
    $$PWD/codedef.h \
    $$PWD/utils.h

SOURCES += \
    $$PWD/coursedef.cpp \
    $$PWD/codedef.cpp \
    $$PWD/utils.cpp

FORMS += \

OTHER_FILES += \

include($$PWD/og/og.pri)
include($$PWD/si/si.pri)
include($$PWD/exporters/exporters.pri)

