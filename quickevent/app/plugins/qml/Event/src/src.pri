message(including $$PWD)

HEADERS += \
    $$PWD/eventplugin.h \
    $$PWD/connectdbdialogwidget.h \
    $$PWD/connectionsettings.h \
    $$PWD/eventconfig.h \
    $$PWD/eventdialogwidget.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/eventplugin.cpp \
    $$PWD/connectdbdialogwidget.cpp \
    $$PWD/connectionsettings.cpp \
    $$PWD/eventconfig.cpp \
    $$PWD/eventdialogwidget.cpp

FORMS += \
    $$PWD/connectdbdialogwidget.ui \
    $$PWD/eventdialogwidget.ui
