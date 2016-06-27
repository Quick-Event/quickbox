TARGET = qfpsql
#TARGET = $${TARGET}$$QF_LIBRARY_DEBUG_EXT

DEFINES += QF_PATCH

PSQL_HOME = $$(PSQL_HOME)

isEmpty(PSQL_HOME) {
	win32 {
		PSQL_HOME = "C:\Program Files (x86)\PostgreSQL\9.3"
	}
}

unix {
	PSQL_HEADERS = /usr/include/postgresql
}
else:!isEmpty(PSQL_HOME) {
	PSQL_HEADERS = $$PSQL_HOME/include
	PSQL_LIBS = $$PSQL_HOME/lib
	win32 {
		# uncomment following line if compiler complains about redefining struct timespec in pthread.h
		#DEFINES += HAVE_STRUCT_TIMESPEC
	}
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
