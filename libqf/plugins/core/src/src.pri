HEADERS += \
    $$PWD/qmllogsingleton.h \
    $$PWD/settings.h \
    $$PWD/crypt.h \
    $$PWD/qmlfilesingleton.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/qmllogsingleton.cpp \
    $$PWD/settings.cpp \
    $$PWD/crypt.cpp \
    $$PWD/qmlfilesingleton.cpp

include ($$PWD/sql/sql.pri)
include ($$PWD/model/model.pri)
