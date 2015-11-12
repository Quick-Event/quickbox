message(including $$PWD)

include($$PWD/Runs/Runs.pri)

HEADERS += \
    $$PWD/runspluginglobal.h \
    $$PWD/thispartwidget.h \
    $$PWD/runswidget.h \
    $$PWD/runstablemodel.h \
    $$PWD/drawing/drawingganttwidget.h \
    $$PWD/drawing/classitem.h \
    $$PWD/drawing/startslotitem.h \
    $$PWD/drawing/ganttitem.h \
    $$PWD/drawing/ganttscene.h \
    $$PWD/drawing/ganttview.h \
    $$PWD/drawing/iganttitem.h \
    $$PWD/drawing/ganttruler.h \
    $$PWD/drawing/startslotheader.h \
    $$PWD/runstableitemdelegate.h \
    $$PWD/runstablewidget.h \
    $$PWD/runstabledialogwidget.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/thispartwidget.cpp \
    $$PWD/runswidget.cpp \
    $$PWD/runstablemodel.cpp \
    $$PWD/drawing/drawingganttwidget.cpp \
    $$PWD/drawing/classitem.cpp \
    $$PWD/drawing/startslotitem.cpp \
    $$PWD/drawing/ganttitem.cpp \
    $$PWD/drawing/ganttscene.cpp \
    $$PWD/drawing/ganttview.cpp \
    $$PWD/drawing/iganttitem.cpp \
    $$PWD/drawing/ganttruler.cpp \
    $$PWD/drawing/startslotheader.cpp \
    $$PWD/runstableitemdelegate.cpp \
    $$PWD/runstablewidget.cpp \
    $$PWD/runstabledialogwidget.cpp

FORMS += \
    $$PWD/runswidget.ui \
    $$PWD/drawing/drawingganttwidget.ui \
    $$PWD/runstablewidget.ui \
    $$PWD/runstabledialogwidget.ui
