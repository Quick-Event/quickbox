message(including $$PWD)

HEADERS += \
	$$PWD/thispartwidget.h \
	$$PWD/speakerwidget.h \

SOURCES += \
	$$PWD/plugin.cpp \
	$$PWD/thispartwidget.cpp \
	$$PWD/speakerwidget.cpp \

FORMS += \
	$$PWD/speakerwidget.ui \

include ( $$PWD/Speaker/Speaker.pri )
