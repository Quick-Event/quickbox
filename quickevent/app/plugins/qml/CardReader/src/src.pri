message(including $$PWD)

HEADERS += \
    $$PWD/cardreaderwidget.h \
    $$PWD/cardreaderpartwidget.h \
    $$PWD/dlgsettings.h \
    $$PWD/cardreaderplugin.h \
    $$PWD/cardcheck.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/cardreaderwidget.cpp \
    $$PWD/cardreaderpartwidget.cpp \
    $$PWD/dlgsettings.cpp \
    $$PWD/cardreaderplugin.cpp \
    $$PWD/cardcheck.cpp

FORMS += \
    $$PWD/cardreaderwidget.ui \
    $$PWD/dlgsettings.ui
