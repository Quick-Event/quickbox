MY_SUBPROJECT = quickshow

message($$MY_SUBPROJECT)

TEMPLATE = app

QT += gui sql widgets

CONFIG += warn_on qt thread

CONFIG += c++11

TARGET = $$MY_SUBPROJECT

QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/..
message ( QF_PROJECT_TOP_BUILDDIR == '$$QF_PROJECT_TOP_BUILDDIR' )

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
message ( DESTDIR: $$DESTDIR )

INCLUDEPATH += $$PWD/../libqf/libqfcore/include

LIBS +=      \
	-lqfcore  \
	-lqfqmlwidgets  \

win32: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/bin  \

unix: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib\' \

CONFIG(debug, debug|release) {
	# exception backtrace support
	unix:QMAKE_LFLAGS += -rdynamic
}

message(LIBS: $$LIBS)

win32: CONFIG += console

RC_FILE = $${MY_SUBPROJECT}.rc

include ($$PWD/src/src.pri)

OTHER_FILES += \

quickshow-android {
CONFIG += mobility
MOBILITY =

deployment.files += quickshow.conf
deployment.path = /assets
INSTALLS += deployment

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        /home/fanda/proj/quickbox/quickshow/../../../programs/qt5/5.9.1/android_armv7/lib/libQt5Xml.so \
        /home/fanda/proj/quickbox/quickshow/../../../programs/qt5/5.9.1/android_armv7/lib/libQt5Qml.so \
        /home/fanda/proj/quickbox/quickshow/../../../programs/qt5/5.9.1/android_armv7/lib/libQt5Network.so \
        /home/fanda/proj/quickbox/quickshow/../../../programs/qt5/5.9.1/android_armv7/lib/libQt5PrintSupport.so \
        $$PWD/../../../programs/qt5/5.9.1/android_armv7/lib/libQt5Svg.so
}
}

TRANSLATIONS += \
	$${TARGET}.cs_CZ.ts \
	$${TARGET}.fr_FR.ts \
	$${TARGET}.nb_NO.ts \
	$${TARGET}.nl_BE.ts \
	$${TARGET}.pl_PL.ts \
	$${TARGET}.ru_RU.ts \
	$${TARGET}.uk_UA.ts \
