HEADERS  += \
	$$PWD/quickeventglobal.h   \
    $$PWD/reportoptionsdialog.h \
    $$PWD/partwidget.h

SOURCES += \
    $$PWD/reportoptionsdialog.cpp \
    $$PWD/partwidget.cpp

FORMS += \
    $$PWD/reportoptionsdialog.ui \

OTHER_FILES += \

include($$PWD/og/og.pri)
include($$PWD/si/si.pri)
include ($$PWD/audio/audio.pri)

