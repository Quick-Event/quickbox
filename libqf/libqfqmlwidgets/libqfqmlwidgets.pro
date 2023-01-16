message("including $$PWD")

QT += widgets qml sql
QT += xml printsupport svg # needed by reports

CONFIG += c++17 hide_symbols
CONFIG += lrelease embed_translations

DEFINES += QFQMLWIDGETS_BUILD_DLL

TEMPLATE = lib
TARGET=qfqmlwidgets

isEmpty(QF_PROJECT_TOP_BUILDDIR) {
	QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../..
}
else {
	message ( QF_PROJECT_TOP_BUILDDIR is not empty and set to $$QF_PROJECT_TOP_BUILDDIR )
	message ( This is obviously done in file $$QF_PROJECT_TOP_SRCDIR/.qmake.conf )
}
message ( QF_PROJECT_TOP_BUILDDIR == '$$QF_PROJECT_TOP_BUILDDIR' )

isEmpty(QF_PROJECT_TOP_SRCDIR) {
    QF_PROJECT_TOP_SRCDIR = $$PWD/../..
}
else {
    message ( QF_PROJECT_TOP_SRCDIR is not empty and set to $$QF_PROJECT_TOP_SRCDIR )
	message ( This is obviously done in file $$QF_PROJECT_TOP_SRCDIR/.qmake.conf )
}
message ( QF_PROJECT_TOP_SRCDIR == '$$QF_PROJECT_TOP_SRCDIR' )

unix:DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/lib
win32:DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin

message ( DESTDIR: $$DESTDIR )

INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/3rdparty/necrolog/include

android: LIBEXT = "_$${QT_ARCH}"
else: LIBEXT = ""

LIBS +=      \
	-lnecrolog$${LIBEXT}  \
	-lqfcore$${LIBEXT}

win32: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/bin  \

unix: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/lib  \

include($$PWD/src/src.pri)

RESOURCES += \
    $$PWD/style/style.qrc \
    $$PWD/images/images.qrc \
    $$PWD/images/flat/flat.qrc

TRANSLATIONS += \
        libqfqmlwidgets-cs_CZ.ts \
	libqfqmlwidgets-fr_FR.ts \
	libqfqmlwidgets-nb_NO.ts \
	libqfqmlwidgets-nl_BE.ts \
	libqfqmlwidgets-pl_PL.ts \
	libqfqmlwidgets-ru_RU.ts \
	libqfqmlwidgets-uk_UA.ts \
