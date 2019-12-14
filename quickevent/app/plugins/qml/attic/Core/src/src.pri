message(including $$PWD)

HEADERS += \
    $$PWD/coreplugin.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/coreplugin.cpp

FORMS += \

include($$PWD/widgets/widgets.pri)
