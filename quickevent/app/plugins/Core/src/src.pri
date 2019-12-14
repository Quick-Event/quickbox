message(including $$PWD)

HEADERS += \
    $$PWD/corepluginglobal.h \
    $$PWD/coreplugin.h

SOURCES += \
    $$PWD/coreplugin.cpp

FORMS += \

include($$PWD/widgets/widgets.pri)
