message(including $$PWD)

HEADERS += \
	$$PWD/eventpluginglobal.h \
    $$PWD/connectdbdialogwidget.h \
    $$PWD/connectionsettings.h \
    $$PWD/eventdialogwidget.h \
    $$PWD/Event/eventplugin.h \
    $$PWD/Event/eventconfig.h \
    $$PWD/Event/stage.h \
    $$PWD/Event/stagedocument.h \
    $$PWD/Event/stagewidget.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/connectdbdialogwidget.cpp \
    $$PWD/connectionsettings.cpp \
    $$PWD/eventdialogwidget.cpp \
    $$PWD/Event/eventplugin.cpp \
    $$PWD/Event/eventconfig.cpp \
    $$PWD/Event/stage.cpp \
    $$PWD/Event/stagedocument.cpp \
    $$PWD/Event/stagewidget.cpp

FORMS += \
    $$PWD/connectdbdialogwidget.ui \
    $$PWD/eventdialogwidget.ui \
    $$PWD/Event/stagewidget.ui
