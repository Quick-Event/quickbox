message(including $$PWD)

HEADERS += \
	$$PWD/eventpluginglobal.h \
    $$PWD/eventplugin.h \
    $$PWD/eventconfig.h \
    $$PWD/stage.h \
    $$PWD/stagedocument.h \
    $$PWD/stagewidget.h \

SOURCES += \
    $$PWD/eventplugin.cpp \
    $$PWD/eventconfig.cpp \
    $$PWD/stage.cpp \
    $$PWD/stagedocument.cpp \
    $$PWD/stagewidget.cpp \

FORMS += \
    $$PWD/stagewidget.ui

include( $$PWD/priv/priv.pri )
include( $$PWD/services/services.pri )
