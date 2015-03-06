message(including $$PWD)

HEADERS += \
	$$PWD/qecorepluginglobal.h \
    $$PWD/coreplugin.h

SOURCES += \
	$$PWD/plugin.cpp \
    $$PWD/coreplugin.cpp

FORMS += \

include($$PWD/ogsupport/ogsupport.pri)
include($$PWD/widgets/widgets.pri)
