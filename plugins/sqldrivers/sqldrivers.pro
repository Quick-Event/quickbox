include(../../top.pri)

TEMPLATE = subdirs

#version check qt
minQtVersion(5, 4, 0) {
    message("Building 5.4.x SQL plugins")
	SUBDIRS = \
		5.4 \
}
else {
    message("Building 5.3.x SQL plugins")
	SUBDIRS = \
		5.3 \
}
