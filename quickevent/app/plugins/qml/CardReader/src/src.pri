message(including $$PWD)

HEADERS += \
    $$PWD/cardreaderwidget.h \
    $$PWD/cardreaderpartwidget.h \
    $$PWD/dlgsettings.h \
    $$PWD/cardreaderplugin.h \
    $$PWD/cardchecker.h \

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/cardreaderwidget.cpp \
    $$PWD/cardreaderpartwidget.cpp \
    $$PWD/dlgsettings.cpp \
    $$PWD/cardreaderplugin.cpp \
    $$PWD/cardchecker.cpp \

FORMS += \
    $$PWD/cardreaderwidget.ui \
    $$PWD/dlgsettings.ui

#include ( $$PWD/check/check.pri )
