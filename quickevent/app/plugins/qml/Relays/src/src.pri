message(including $$PWD)

HEADERS += \
	$$PWD/competitorspluginglobal.h \
    $$PWD/thispartwidget.h \
    $$PWD/registrationswidget.h \
    $$PWD/findregistrationedit.h \
    $$PWD/lentcardswidget.h \
    $$PWD/relayswidget.h \
    $$PWD/relaywidget.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/thispartwidget.cpp \
    $$PWD/registrationswidget.cpp \
    $$PWD/findregistrationedit.cpp \
    $$PWD/lentcardswidget.cpp \
    $$PWD/relayswidget.cpp \
    $$PWD/relaywidget.cpp

FORMS += \
    $$PWD/registrationswidget.ui \
    $$PWD/lentcardswidget.ui \
    $$PWD/relayswidget.ui \
    $$PWD/relaywidget.ui

include ( $$PWD/Relays/Relays.pri )
