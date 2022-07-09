message(including $$PWD)

HEADERS += \
    $$PWD/cardcheckerfreeordercpp.h \
	$$PWD/cardreadersettingspage.h \
    $$PWD/cardreaderwidget.h \
    $$PWD/cardreaderplugin.h \
    $$PWD/cardchecker.h \
    $$PWD/cardcheckerclassiccpp.h \

SOURCES += \
    $$PWD/cardcheckerfreeordercpp.cpp \
	$$PWD/cardreadersettingspage.cpp \
    $$PWD/cardreaderwidget.cpp \
    $$PWD/cardreaderplugin.cpp \
    $$PWD/cardchecker.cpp \
    $$PWD/cardcheckerclassiccpp.cpp \

FORMS += \
    $$PWD/cardreadersettingspage.ui \
    $$PWD/cardreaderwidget.ui

include ( $$PWD/services/services.pri )
