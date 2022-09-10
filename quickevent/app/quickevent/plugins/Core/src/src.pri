message(including $$PWD)

HEADERS += \
    $$PWD/coreplugin.h \
    $$PWD/reportssettings.h \
    $$PWD/settings.h

SOURCES += \
    $$PWD/coreplugin.cpp \
    $$PWD/reportssettings.cpp \
    $$PWD/settings.cpp

FORMS += \

include($$PWD/widgets/widgets.pri)

