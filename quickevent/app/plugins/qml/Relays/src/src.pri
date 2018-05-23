message(including $$PWD)

HEADERS += \
	$$PWD/relayspluginglobal.h \
    $$PWD/thispartwidget.h \
    $$PWD/relayswidget.h \
    $$PWD/relaywidget.h \
    $$PWD/addlegdialogwidget.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/thispartwidget.cpp \
    $$PWD/relayswidget.cpp \
    $$PWD/relaywidget.cpp \
    $$PWD/addlegdialogwidget.cpp

FORMS += \
    $$PWD/relayswidget.ui \
    $$PWD/relaywidget.ui \
    $$PWD/addlegdialogwidget.ui

include ( $$PWD/Relays/Relays.pri )
