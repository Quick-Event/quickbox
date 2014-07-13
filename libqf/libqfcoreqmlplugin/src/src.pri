message(including $$PWD)

INCLUDEPATH += $$PWD/../../libqfcore/include

HEADERS += \
    $$PWD/qmllogsingleton.h \
    $$PWD/settings.h \
    $$PWD/crypt.h \

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/qmllogsingleton.cpp \
    $$PWD/settings.cpp \
    $$PWD/crypt.cpp \

include ($$PWD/sql/sql.pri)
include ($$PWD/model/model.pri)
include ($$PWD/network/network.pri)
