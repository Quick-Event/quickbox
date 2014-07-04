HEADERS += \
	$$PWD/coreglobal.h \
	$$PWD/assert.h \
	$$PWD/exception.h \
	$$PWD/stacktrace.h \
	$$PWD/log.h \
	$$PWD/utils.h \
	$$PWD/string.h \
    $$PWD/logdevice.h \

SOURCES += \
	$$PWD/exception.cpp \
	$$PWD/stacktrace.cpp \
	$$PWD/log.cpp \
	$$PWD/string.cpp \
    $$PWD/logdevice.cpp \
    $$PWD/utils.cpp

include ($$PWD/utils/utils.pri)
include ($$PWD/sql/sql.pri)
