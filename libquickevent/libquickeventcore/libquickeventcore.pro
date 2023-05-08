TEMPLATE = lib
TARGET = quickeventcore

QT += core sql qml

CONFIG += qt dll
CONFIG += c++17 hide_symbols
CONFIG += lrelease embed_translations

DEFINES += QUICKEVENTCORE_BUILD_DLL

unix:DESTDIR = $$OUT_PWD/../../lib
win32:DESTDIR = $$OUT_PWD/../../bin

message(Target: $$TARGET)

LIBS += -L$$DESTDIR
LIBS += -lnecrolog
LIBS += -lqfcore
LIBS += -lsiut

INCLUDEPATH += \
    $$PWD/../../3rdparty/necrolog/include \
	$$PWD/../../libqf/libqfcore/include \
	$$PWD/../../libsiut/include

message(INCLUDEPATH: $$INCLUDEPATH)

include ( src/src.pri )

RESOURCES += libquickeventcore.qrc

TRANSLATIONS += \
        libquickeventcore-cs_CZ.ts \
	libquickeventcore-fr_FR.ts \
	libquickeventcore-nb_NO.ts \
	libquickeventcore-nl_BE.ts \
	libquickeventcore-pl_PL.ts \
	libquickeventcore-ru_RU.ts \
	libquickeventcore-uk_UA.ts \
