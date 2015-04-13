message(including $$PWD)

HEADERS += \
    $$PWD/runspluginglobal.h \
    $$PWD/runsplugin.h \
    $$PWD/thispartwidget.h \
    $$PWD/runswidget.h \
    $$PWD/runstablemodel.h \
    $$PWD/drawing/drawingtoolwidget.h \
    $$PWD/drawing/classitem.h \
    $$PWD/drawing/slotitem.h \
    $$PWD/drawing/ganttitem.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/runsplugin.cpp \
    $$PWD/thispartwidget.cpp \
    $$PWD/runswidget.cpp \
    $$PWD/runstablemodel.cpp \
    $$PWD/drawing/drawingtoolwidget.cpp \
    $$PWD/drawing/classitem.cpp \
    $$PWD/drawing/slotitem.cpp \
    $$PWD/drawing/ganttitem.cpp

FORMS += \
    $$PWD/runswidget.ui \
    $$PWD/drawing/drawingtoolwidget.ui
