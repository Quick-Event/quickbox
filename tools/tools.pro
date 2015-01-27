TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
	qsqlmon \

unix {
SUBDIRS += \
	qfsqldbfs \
}


