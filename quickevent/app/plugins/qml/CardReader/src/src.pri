message(including $$PWD)

HEADERS += \
    $$PWD/cardreaderwidget.h \
    $$PWD/cardreaderpartwidget.h \
    $$PWD/dlgsettings.h \
    $$PWD/CardReader/cardreaderplugin.h \
    $$PWD/CardReader/cardchecker.h \
    $$PWD/CardReader/cardcheckerclassiccpp.h \

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/cardreaderwidget.cpp \
    $$PWD/cardreaderpartwidget.cpp \
    $$PWD/dlgsettings.cpp \
    $$PWD/CardReader/cardreaderplugin.cpp \
    $$PWD/CardReader/cardchecker.cpp \
    $$PWD/CardReader/cardcheckerclassiccpp.cpp \

FORMS += \
    $$PWD/cardreaderwidget.ui \
    $$PWD/dlgsettings.ui

include ( $$PWD/services/services.pri )
