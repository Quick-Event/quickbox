TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
	libqfcore \
	libqfqmlwidgets \

!static-build {
SUBDIRS += \
	plugins \
}

