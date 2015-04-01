message(including $$PWD)

HEADERS += \
	$$PWD/eventpluginglobal.h \
    $$PWD/eventplugin.h \
    $$PWD/connectdbdialogwidget.h \
    $$PWD/connectionsettings.h \
    $$PWD/Event/eventconfig.h \
    $$PWD/eventdialogwidget.h \
    $$PWD/Event/stagedata.h \
    $$PWD/Event/stagedocument.h \
    $$PWD/Event/stagewidget.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/eventplugin.cpp \
    $$PWD/connectdbdialogwidget.cpp \
    $$PWD/connectionsettings.cpp \
    $$PWD/Event/eventconfig.cpp \
    $$PWD/eventdialogwidget.cpp \
    $$PWD/Event/stagedata.cpp \
    $$PWD/Event/stagedocument.cpp \
    $$PWD/Event/stagewidget.cpp

FORMS += \
    $$PWD/connectdbdialogwidget.ui \
    $$PWD/eventdialogwidget.ui \
    $$PWD/Event/stagewidget.ui
