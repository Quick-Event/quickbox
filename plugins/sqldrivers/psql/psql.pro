TARGET = qfpsql
#TARGET = $${TARGET}$$QF_LIBRARY_DEBUG_EXT

DEFINES += QF_PATCH
unix:
    PSQL_HEADERS = /usr/include/postgresql # fanda QF_PATCH

win32 {
    PSQL_HOME = C:\app\psql
    PSQL_HEADERS = $$PSQL_HOME\include
    PSQL_LIBS = $$PSQL_HOME\lib
}

HEADERS += \
	qsql_psql_p.h \

SOURCES += \
	main.cpp \
	qsql_psql.cpp \

LIBS += -lpq

win32 {
    LIBS += -L$$PSQL_LIBS
}

INCLUDEPATH += \
        $$PSQL_HEADERS \

include(../qsqldriverbase.pri)
