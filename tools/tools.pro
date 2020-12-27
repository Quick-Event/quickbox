TEMPLATE = subdirs
CONFIG += ordered

#message (tools config: $$CONFIG)

SUBDIRS += \
	qsqlmon \

unix:qfsqldbfs {
	SUBDIRS += \
		qfsqldbfs \
}

