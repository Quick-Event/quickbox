TARGET = qfpsql
#TARGET = $${TARGET}$$QF_LIBRARY_DEBUG_EXT

DEFINES += QF_PATCH
unix {
    PSQL_HEADERS = /usr/include/postgresql
}
else:win32 {
    message(windows build)
    PSQL_HOME = "C:\Program Files (x86)\PostgreSQL\9.3"
    PSQL_HEADERS = $$PSQL_HOME\include
    PSQL_LIBS = $$PSQL_HOME\lib

    # uncomment following line if compiler complains about redefining struct timespec in pthread.h
    #DEFINES += HAVE_STRUCT_TIMESPEC
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

include(../../qsqldriverbase.pri)
