TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
	core \
	qmlwidgets \ # cannot be removed, Windows cannot load qml module (all includes this one)
	qmlreports \


