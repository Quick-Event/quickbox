include(top.pri)

#version check qt
!minQtVersion(5, 2, 1) {
	message("Cannot build QuickBox with Qt version $${QT_VERSION}.")
	error("Use at least Qt 5.3.1.")
}

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
	libqf \

qfsqldbfs {
	SUBDIRS += \
		tools \

}
else:qsqlmon {
	SUBDIRS += \
		tools \

}
else {
	SUBDIRS += \
		libsiut \
		tools \
		quickevent \
		qsishow \

}

