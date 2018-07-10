TEMPLATE = subdirs
CONFIG += ordered

#message (tools config: $$CONFIG)

SUBDIRS += \
	qsqlmon \

qfsqldbfs {
	SUBDIRS += \
		qfsqldbfs \
}

