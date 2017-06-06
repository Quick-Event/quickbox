message(including $$PWD)

HEADERS += \
	$$PWD/thispartwidget.h \
	$$PWD/speakerwidget.h \
    $$PWD/codeclassresultswidget.h \
    $$PWD/codeclassresultsgridwidget.h \
    $$PWD/punchestableview.h

SOURCES += \
	$$PWD/plugin.cpp \
	$$PWD/thispartwidget.cpp \
	$$PWD/speakerwidget.cpp \
    $$PWD/codeclassresultswidget.cpp \
    $$PWD/codeclassresultsgridwidget.cpp \
    $$PWD/punchestableview.cpp

FORMS += \
	$$PWD/speakerwidget.ui \
    $$PWD/codeclassresultswidget.ui

include ( $$PWD/Speaker/Speaker.pri )
