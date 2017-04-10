message(including $$PWD)

include($$PWD/Runs/Runs.pri)

HEADERS += \
    $$PWD/runspluginglobal.h \
    $$PWD/thispartwidget.h \
    $$PWD/runswidget.h \
    $$PWD/runstablemodel.h \
    $$PWD/runstableitemdelegate.h \
    $$PWD/runstablewidget.h \
    $$PWD/runstabledialogwidget.h \
    $$PWD/eventstatisticswidget.h \
    $$PWD/printawardsoptionsdialogwidget.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/thispartwidget.cpp \
    $$PWD/runswidget.cpp \
    $$PWD/runstablemodel.cpp \
    $$PWD/runstableitemdelegate.cpp \
    $$PWD/runstablewidget.cpp \
    $$PWD/runstabledialogwidget.cpp \
    $$PWD/eventstatisticswidget.cpp \
    $$PWD/printawardsoptionsdialogwidget.cpp

FORMS += \
    $$PWD/runswidget.ui \
    $$PWD/runstablewidget.ui \
    $$PWD/runstabledialogwidget.ui \
    $$PWD/eventstatisticswidget.ui \
    $$PWD/printawardsoptionsdialogwidget.ui
