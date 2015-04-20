message(including plugin $$PWD)

PLUGIN_NAME = Runs

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

CONFIG += c++11 hide_symbols

DEFINES += RUNSPLUGIN_BUILD_DLL

INCLUDEPATH += \
    $$PWD/src \
    $$PWD/../Event/include \

LIBS += \
    -L$$DESTDIR \
    -lEventplugin \


include (src/src.pri)

RESOURCES += \
#    $${PLUGIN_NAME}.qrc \

OTHER_FILES += \
	$$PWD/qml/reports/* \

#DISTFILES += \
#    qml/RunsModel.qml
