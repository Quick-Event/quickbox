TARGET = qfpsql
#TARGET = $${TARGET}$$QF_LIBRARY_DEBUG_EXT

DEFINES += QF_PATCH
unix:PSQL_DIR = /usr/include/postgresql # fanda QF_PATCH
win32:PSQL_DIR = c:/app/postgresql9 # fanda QF_PATCH

HEADERS += \
	qsql_psql_p.h \

SOURCES += \
	main.cpp \
	qsql_psql.cpp \

unix|mingw {
    LIBS += $$QT_LFLAGS_PSQL
    !contains(LIBS, .*pq.*):LIBS += -lpq
    QMAKE_CXXFLAGS *= $$QT_CFLAGS_PSQL
} else {
    !contains(LIBS, .*pq.*):LIBS += -llibpq -lws2_32 -ladvapi32
}

INCLUDEPATH += \
	$$MY_PATH_TO_TOP/include \

unix {
	INCLUDEPATH += \
		$$PSQL_DIR
}

include(../qsqldriverbase.pri)
