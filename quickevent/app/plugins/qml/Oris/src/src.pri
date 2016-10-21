message(including $$PWD)

HEADERS += \
    $$PWD/oris.h \
    $$PWD/orisimporter.h \
    $$PWD/chooseoriseventdialog.h \
    $$PWD/txtimporter.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/oris.cpp \
    $$PWD/orisimporter.cpp \
    $$PWD/chooseoriseventdialog.cpp \
    $$PWD/txtimporter.cpp

FORMS += \
    $$PWD/chooseoriseventdialog.ui
