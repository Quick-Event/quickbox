message("including $$PWD")

TEMPLATE = lib
TARGET=qfcore

#message (QF_PROJECT_TOP_SRCDIR: $$QF_PROJECT_TOP_SRCDIR)
#message (QF_PROJECT_TOP_BUILDDIR: $$QF_PROJECT_TOP_BUILDDIR)

isEmpty(QF_PROJECT_TOP_BUILDDIR) {
	QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../..
}
else {
	message ( QF_PROJECT_TOP_BUILDDIR is not empty and set to $$QF_PROJECT_TOP_BUILDDIR )
	message ( This is obviously done in file $$QF_PROJECT_TOP_SRCDIR/.qmake.conf )
}
message ( QF_PROJECT_TOP_BUILDDIR == '$$QF_PROJECT_TOP_BUILDDIR' )

unix:DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/lib
win32:DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin

message ( DESTDIR: $$DESTDIR )

QT += xml sql qml
CONFIG += C++11
CONFIG += hide_symbols

DEFINES += QFCORE_BUILD_DLL

include($$PWD/src/src.pri)

include ($$PWD/../../crosscompile-support.pri)

RESOURCES += \
	$$PWD/images/images.qrc \

TRANSLATIONS += \
	libqfcore.cs_CZ.ts \
	libqfcore.pl_PL.ts \
	libqfcore.nb_NO.ts \
	libqfcore.ru_RU.ts \
	libqfcore.nl_BE.ts \
