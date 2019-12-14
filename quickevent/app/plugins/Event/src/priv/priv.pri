message(including $$PWD)

HEADERS += \
    $$PWD/connectdbdialogwidget.h \
    $$PWD/connectionsettings.h \
    $$PWD/eventdialogwidget.h \
    $$PWD/dbschema.h \

SOURCES += \
    $$PWD/connectdbdialogwidget.cpp \
    $$PWD/connectionsettings.cpp \
    $$PWD/eventdialogwidget.cpp \
    $$PWD/dbschema.cpp \

FORMS += \
    $$PWD/connectdbdialogwidget.ui \
    $$PWD/eventdialogwidget.ui \

include( $$PWD/services/services.pri )
