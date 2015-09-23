TEMPLATE = subdirs
CONFIG += ordered

#message (tools config: $$CONFIG)

SUBDIRS += \
	qsqlmon \

unix {
	!qsqlmon {
		SUBDIRS += \
			qfsqldbfs \
	}
}

