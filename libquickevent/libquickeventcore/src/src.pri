HEADERS  += \
    $$PWD/quickeventcoreglobal.h   \
    $$PWD/coursedef.h \
    $$PWD/codedef.h \

SOURCES += \
    $$PWD/coursedef.cpp \
    $$PWD/codedef.cpp \

FORMS += \

OTHER_FILES += \

include($$PWD/og/og.pri)
include($$PWD/si/si.pri)
include($$PWD/exporters/exporters.pri)

