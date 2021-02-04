include(utils.pri)

#version check qt
!minQtVersion(5, 11, 0) {
	message("Cannot build QuickBox with Qt version $${QT_VERSION}.")
	error("Use at least Qt 5.11.0.")
}

TEMPLATE = subdirs
CONFIG += ordered

#message (config: $$CONFIG)

SUBDIRS += \
	3rdparty \
	libqf \

quickshow-android {
	SUBDIRS += \
		quickshow \
}
else {
	SUBDIRS += \
		libsiut \
		libquickevent \
		quickevent \
		quickhttpd \
		quickshow \
		tools \

}

OTHER_FILES += \
	.travis.yml

