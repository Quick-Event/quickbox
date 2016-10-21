include(top.pri)

#version check qt
!minQtVersion(5, 5, 0) {
	message("Cannot build QuickBox with Qt version $${QT_VERSION}.")
	error("Use at least Qt 5.5.0.")
}

TEMPLATE = subdirs
CONFIG += ordered

#message (config: $$CONFIG)

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
		quickshow-qml \
		quickshow \

}

