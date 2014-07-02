message(including $$PWD)

INCLUDEPATH += $$PWD/../../libqfcore/include

HEADERS += \
    $$PWD/qmlsqlsingleton.h \
    $$PWD/sqldatabase.h \
    $$PWD/sqlquery.h \
    $$PWD/sqlrecord.h \
    $$PWD/sqlquerybuilder.h

SOURCES += \
    $$PWD/qmlsqlsingleton.cpp \
    $$PWD/sqldatabase.cpp \
    $$PWD/sqlquery.cpp \
    $$PWD/sqlrecord.cpp \
    $$PWD/sqlquerybuilder.cpp

