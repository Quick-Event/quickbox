TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
	src \
	plugins \


OTHER_FILES += \
    $$PWD/plugins/qml/SqlDb/* \
