include(top.pri)

#version check qt
!minQtVersion(5, 3, 1) {
    message("Cannot build QuickBox with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.3.1.")
}

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
	libqf \
	plugins \
	libsiut \
    tools \
    qsicli \
	quickevent \


