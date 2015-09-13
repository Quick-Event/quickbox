HEADERS += \
	$$PWD/coreglobal.h \
	$$PWD/assert.h \
	$$PWD/exception.h \
	$$PWD/stacktrace.h \
	$$PWD/log.h \
	$$PWD/logcore.h \
	$$PWD/utils.h \
	$$PWD/string.h \
    $$PWD/logdevice.h \
    $$PWD/collator.h \

SOURCES += \
	$$PWD/exception.cpp \
	$$PWD/stacktrace.cpp \
	$$PWD/logcore.cpp \
	$$PWD/string.cpp \
    $$PWD/logdevice.cpp \
    $$PWD/utils.cpp \
    $$PWD/collator.cpp \

unix {
HEADERS += \
	$$PWD/syslogdevice.h

SOURCES += \
	$$PWD/syslogdevice.cpp
}
