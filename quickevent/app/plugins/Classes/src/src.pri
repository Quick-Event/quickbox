message(including $$PWD)

HEADERS += \
	$$PWD/classespluginglobal.h \
    $$PWD/classesplugin.h \
    $$PWD/classdocument.h \
    $$PWD/importcoursedef.h \
    $$PWD/thispartwidget.h \
    $$PWD/classeswidget.h \
    $$PWD/editcodeswidget.h \
    $$PWD/editcourseswidget.h \
    $$PWD/editcoursecodeswidget.h \
    $$PWD/classestableview.h \
    $$PWD/classdefdocument.h \
    $$PWD/classdefwidget.h

SOURCES += \
    $$PWD/classesplugin.cpp \
    $$PWD/classdocument.cpp \
    $$PWD/importcoursedef.cpp \
    $$PWD/thispartwidget.cpp \
    $$PWD/classeswidget.cpp \
    $$PWD/editcodeswidget.cpp \
    $$PWD/editcourseswidget.cpp \
    $$PWD/editcoursecodeswidget.cpp \
    $$PWD/classestableview.cpp \
    $$PWD/classdefdocument.cpp \
    $$PWD/classdefwidget.cpp

FORMS += \
    $$PWD/classeswidget.ui \
    $$PWD/editcodeswidget.ui \
    $$PWD/editcourseswidget.ui \
    $$PWD/editcoursecodeswidget.ui \
    $$PWD/classdefwidget.ui

include ( $$PWD/drawing/drawing.pri )
