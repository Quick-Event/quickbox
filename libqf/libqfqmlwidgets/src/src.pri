INCLUDEPATH += $$PWD/../../libqfcore/include

HEADERS += \
	$$PWD/qmlwidgetsglobal.h \
	$$PWD/frame.h \
	$$PWD/label.h \
    $$PWD/action.h \
    $$PWD/menu.h \
    $$PWD/menubar.h \
    $$PWD/dialog.h \
    $$PWD/lineedit.h \
    $$PWD/layoutpropertiesattached.h \
    $$PWD/splitter.h \
    $$PWD/gridlayoutproperties.h

SOURCES += \
	$$PWD/frame.cpp \
	$$PWD/label.cpp \
    $$PWD/action.cpp \
    $$PWD/menu.cpp \
    $$PWD/menubar.cpp \
    $$PWD/dialog.cpp \
    $$PWD/lineedit.cpp \
    $$PWD/layoutpropertiesattached.cpp \
    $$PWD/splitter.cpp \
    $$PWD/gridlayoutproperties.cpp

include ($$PWD/framework/framework.pri)
