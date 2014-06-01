message("including include/core")

TEMPLATE = lib
unix:TARGET = $$OUT_PWD/../../lib/qfcore

DEFINES += QFCORE_BUILD_DLL

INCLUDEPATH += $$PWD/include

HEADERS += \
	$$PWD/include/qf/core/exception.h \
	$$PWD/include/qf/core/assert.h \
	$$PWD/include/qf/core/stacktrace.h \
	$$PWD/include/qf/core/coreglobal.h \
	$$PWD/include/qf/core/log.h \
	$$PWD/include/qf/core/logcust.h \
	$$PWD/include/qf/core/string.h \
	$$PWD/include/qf/core/logdevice.h
	$$PWD/include/qf/core/utils.h


include($$PWD/src/src.pri)
