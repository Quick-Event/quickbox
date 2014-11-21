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
    $$PWD/layouttypeproperties.h \
#    $$PWD/gridlayouttypeproperties.h \
#    $$PWD/boxlayouttypeproperties.h \
    $$PWD/dialogbuttonbox.h \
    $$PWD/statusbar.h \
    $$PWD/progressbar.h \
    $$PWD/tableview.h \
    $$PWD/headerview.h \
    $$PWD/tableitemdelegate.h \
    $$PWD/tableviewtoolbar.h \
    $$PWD/saveoptionswidget.h \
    $$PWD/toolbar.h \
    $$PWD/datacontroller.h \
    $$PWD/idatawidget.h \
    $$PWD/combobox.h \

SOURCES += \
	$$PWD/frame.cpp \
	$$PWD/label.cpp \
    $$PWD/action.cpp \
    #$$PWD/menu.cpp \
    $$PWD/menubar.cpp \
    $$PWD/lineedit.cpp \
    $$PWD/layoutpropertiesattached.cpp \
    $$PWD/splitter.cpp \
    $$PWD/layouttypeproperties.cpp \
    #$$PWD/gridlayouttypeproperties.cpp \
#    $$PWD/boxlayouttypeproperties.cpp \
    $$PWD/dialogbuttonbox.cpp \
    $$PWD/statusbar.cpp \
    $$PWD/progressbar.cpp \
    $$PWD/tableview.cpp \
    $$PWD/headerview.cpp \
    $$PWD/tableitemdelegate.cpp \
    $$PWD/tableviewtoolbar.cpp \
    $$PWD/saveoptionswidget.cpp \
    $$PWD/toolbar.cpp \
    $$PWD/datacontroller.cpp \
    $$PWD/idatawidget.cpp \
    $$PWD/combobox.cpp \

FORMS += \
    $$PWD/saveoptionswidget.ui \

include ($$PWD/internal/internal.pri)
include ($$PWD/framework/framework.pri)
include ($$PWD/dialogs/dialogs.pri)
include ($$PWD/reports/reports.pri)
include ($$PWD/graphics/graphics.pri)
