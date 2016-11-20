TEMPLATE = subdirs
CONFIG += ordered

#message (QF_PROJECT_BUILD_ROOT $(QF_PROJECT_BUILD_ROOT) )

unix {
SUBDIRS += \
	libxlsxwriter \
}
#message (top_srcdir: $$top_srcdir)
#message (top_builddir: $$top_builddir)
