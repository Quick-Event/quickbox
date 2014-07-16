INCLUDEPATH += $$PWD/../../libqfcore/include

HEADERS += \
	$$PWD/qmlwidgetsglobal.h \
	$$PWD/frame.h \
	$$PWD/label.h \
    $$PWD/action.h \
    #$$PWD/menu.h \
    $$PWD/menubar.h \
    $$PWD/lineedit.h \
    $$PWD/layoutpropertiesattached.h \
    $$PWD/splitter.h \
    $$PWD/gridlayoutproperties.h \
    $$PWD/dialogbuttonbox.h \
    $$PWD/statusbar.h \
    $$PWD/progressbar.h \
    $$PWD/tableview.h \
    $$PWD/headerview.h \
    $$PWD/tableitemdelegate.h \
    $$PWD/tableviewtoolbar.h

SOURCES += \
	$$PWD/frame.cpp \
	$$PWD/label.cpp \
    $$PWD/action.cpp \
    #$$PWD/menu.cpp \
    $$PWD/menubar.cpp \
    $$PWD/lineedit.cpp \
    $$PWD/layoutpropertiesattached.cpp \
    $$PWD/splitter.cpp \
    $$PWD/gridlayoutproperties.cpp \
    $$PWD/dialogbuttonbox.cpp \
    $$PWD/statusbar.cpp \
    $$PWD/progressbar.cpp \
    $$PWD/tableview.cpp \
    $$PWD/headerview.cpp \
    $$PWD/tableitemdelegate.cpp \
    $$PWD/tableviewtoolbar.cpp

include ($$PWD/framework/framework.pri)
include ($$PWD/dialogs/dialogs.pri)
