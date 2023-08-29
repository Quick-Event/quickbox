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

QT += xml sql qml
CONFIG += c++17
CONFIG += hide_symbols
CONFIG += lrelease embed_translations

DEFINES += QFCORE_BUILD_DLL

INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/3rdparty/necrolog/include

android: LIBEXT = "_$${QT_ARCH}"
else: LIBEXT = ""

LIBS +=      \
	-lnecrolog$${LIBEXT}

win32: LIBS +=  \
    -L$$QF_PROJECT_TOP_BUILDDIR/bin  \

unix: LIBS +=  \
    -L$$QF_PROJECT_TOP_BUILDDIR/lib  \

include($$PWD/src/src.pri)

RESOURCES += \
	$$PWD/images/qfcore_images.qrc \

TRANSLATIONS += \
	libqfcore-cs_CZ.ts \
	libqfcore-fr_FR.ts \
	libqfcore-nb_NO.ts \
	libqfcore-nl_BE.ts \
	libqfcore-pl_PL.ts \
	libqfcore-ru_RU.ts \
	libqfcore-uk_UA.ts \
