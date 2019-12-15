message(including $$PWD)

HEADERS += \
	$$PWD/eventpluginglobal.h \
    $$PWD/eventplugin.h \
    $$PWD/eventconfig.h \
    $$PWD/stage.h \
    $$PWD/stagedocument.h \
    $$PWD/stagewidget.h \
    $$PWD/eventdialogwidget.h \
    $$PWD/connectdbdialogwidget.h \
    $$PWD/connectionsettings.h \
    $$PWD/dbschema.h \

SOURCES += \
    $$PWD/eventplugin.cpp \
    $$PWD/eventconfig.cpp \
    $$PWD/stage.cpp \
    $$PWD/stagedocument.cpp \
    $$PWD/stagewidget.cpp \
    $$PWD/eventdialogwidget.cpp \
    $$PWD/connectdbdialogwidget.cpp \
    $$PWD/connectionsettings.cpp \
    $$PWD/dbschema.cpp \

FORMS += \
    $$PWD/stagewidget.ui

include( $$PWD/services/services.pri )
