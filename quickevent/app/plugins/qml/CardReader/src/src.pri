message(including $$PWD)

HEADERS += \
    $$PWD/cardreaderwidget.h \
    $$PWD/cardreaderpartwidget.h \
    $$PWD/dlgsettings.h \
    $$PWD/CardReader/cardreaderplugin.h \
    $$PWD/CardReader/cardchecker.h \
    $$PWD/CardReader/checkedcard.h \
    $$PWD/CardReader/checkedpunch.h \
    $$PWD/CardReader/readcard.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/cardreaderwidget.cpp \
    $$PWD/cardreaderpartwidget.cpp \
    $$PWD/dlgsettings.cpp \
    $$PWD/CardReader/cardreaderplugin.cpp \
    $$PWD/CardReader/cardchecker.cpp \
    $$PWD/CardReader/checkedcard.cpp \
    $$PWD/CardReader/checkedpunch.cpp \
    $$PWD/CardReader/readcard.cpp

FORMS += \
    $$PWD/cardreaderwidget.ui \
    $$PWD/dlgsettings.ui

#include ( $$PWD/check/check.pri )
