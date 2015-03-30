TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
	core \
	#qmlwidgets \ qmlwidgets plugin excluded from build till QTBUG-39477 will be fixed
	qmlreports \


