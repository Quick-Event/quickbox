message(including $$PWD)

HEADERS += \
    $$PWD/cardreaderwidget.h \
    $$PWD/cardreaderpartwidget.h \
    $$PWD/dlgsettings.h \
    $$PWD/cardreaderplugin.h \
    $$PWD/cardchecker.h \
    $$PWD/cardcheckerclassiccpp.h \

SOURCES += \
    $$PWD/cardreaderwidget.cpp \
    $$PWD/cardreaderpartwidget.cpp \
    $$PWD/dlgsettings.cpp \
    $$PWD/cardreaderplugin.cpp \
    $$PWD/cardchecker.cpp \
    $$PWD/cardcheckerclassiccpp.cpp \

FORMS += \
    $$PWD/cardreaderwidget.ui \
    $$PWD/dlgsettings.ui

include ( $$PWD/services/services.pri )
