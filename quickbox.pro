include(utils.pri)

#version check qt
!minQtVersion(5, 2, 1) {
	message("Cannot build QuickBox with Qt version $${QT_VERSION}.")
	error("Use at least Qt 5.3.1.")
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
else:quickshow-android {
	SUBDIRS += \
		quickshow \

}
else {
	SUBDIRS += \
		libsiut \
		tools \
		quickevent \
		quickhttpd \
		quickshow \

}

OTHER_FILES += \
	.travis.yml

