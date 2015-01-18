message(including $$PWD)

HEADERS += \
	$$PWD/qecorepluginglobal.h \

SOURCES += \
	$$PWD/plugin.cpp \

FORMS += \

include($$PWD/ogsupport/ogsupport.pri)
include($$PWD/widgets/widgets.pri)
