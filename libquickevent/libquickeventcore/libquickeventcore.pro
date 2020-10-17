TEMPLATE = lib
TARGET = quickeventcore

QT += core sql qml

CONFIG += qt dll
CONFIG += c++11 hide_symbols

DEFINES += QUICKEVENTCORE_BUILD_DLL

unix:DESTDIR = $$OUT_PWD/../../lib
win32:DESTDIR = $$OUT_PWD/../../bin

message(Target: $$TARGET)

LIBS += -L$$DESTDIR
LIBS += -lqfcore
LIBS += -lsiut

INCLUDEPATH += \
	$$PWD/../../libqf/libqfcore/include \
	$$PWD/../../libsiut/include

message(INCLUDEPATH: $$INCLUDEPATH)

include ( src/src.pri )

RESOURCES += libquickeventcore.qrc

TRANSLATIONS += \
	libquickeventcore.cs_CZ.ts \
	libquickeventcore.pl_PL.ts \
	libquickeventcore.nb_NO.ts \
	libquickeventcore.ru_RU.ts \
	libquickeventcore.nl_BE.ts \
	libquickeventcore.uk_UA.ts \
