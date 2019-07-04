message(including $$PWD)

HEADERS += \
	$$PWD/competitorspluginglobal.h \
    $$PWD/thispartwidget.h \
    $$PWD/competitorswidget.h \
    $$PWD/competitorwidget.h \
    $$PWD/registrationswidget.h \
    $$PWD/findregistrationedit.h \
    $$PWD/lentcardswidget.h \
    $$PWD/stationsbackupmemorywidget.h \

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/thispartwidget.cpp \
    $$PWD/competitorswidget.cpp \
    $$PWD/competitorwidget.cpp \
    $$PWD/registrationswidget.cpp \
    $$PWD/findregistrationedit.cpp \
    $$PWD/lentcardswidget.cpp \
    $$PWD/stationsbackupmemorywidget.cpp \

FORMS += \
    $$PWD/competitorswidget.ui \
    $$PWD/competitorwidget.ui \
    $$PWD/registrationswidget.ui \
    $$PWD/lentcardswidget.ui \
    $$PWD/stationsbackupmemorywidget.ui \

include ( $$PWD/Competitors/Competitors.pri )
