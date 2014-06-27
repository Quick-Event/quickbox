message(including $$PWD)

INCLUDEPATH += $$PWD/../../libqfcore/include

HEADERS += \
    $$PWD/qmllogsingleton.h \
    $$PWD/settings.h \
    $$PWD/crypt.h \
    $$PWD/qmlsqlsingleton.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/qmllogsingleton.cpp \
    $$PWD/settings.cpp \
    $$PWD/crypt.cpp \
    $$PWD/qmlsqlsingleton.cpp

