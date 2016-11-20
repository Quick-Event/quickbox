#isEmpty(QF_PROJECT_TOP_BUILDDIR) {
#	error ( "QF_PROJECT_TOP_BUILDDIR should be set" )
#}
#message ( QF_PROJECT_TOP_BUILDDIR: '$$QF_PROJECT_TOP_BUILDDIR' )

LIBXLSXWRITER_BUILDDIR = $$OUT_PWD/_build

TEMPLATE = aux

#clang {
#message (clang FreeOpcUa build)
#CMAKE_CC_DEF = -DCMAKE_C_COMPILER=/usr/bin/clang
#CMAKE_CXX_DEF = -DCMAKE_CXX_COMPILER=/usr/bin/clang++
#}

libxlsxwriter_conf.commands = \
    rm -rf $$LIBXLSXWRITER_BUILDDIR \
    && mkdir -p $$LIBXLSXWRITER_BUILDDIR \
    && cp -a $$PWD/libxlsxwriter/include $$PWD/libxlsxwriter/src $$PWD/libxlsxwriter/third_party $$PWD/libxlsxwriter/lib $$PWD/libxlsxwriter/Makefile $$LIBXLSXWRITER_BUILDDIR/

libxlsxwriter_build.commands = \
    make -C $$LIBXLSXWRITER_BUILDDIR
#    #&& strip $$QF_PROJECT_TOP_BUILDDIR/lib/opcua.so \

libxlsxwriter_build.depends = libxlsxwriter_conf

QMAKE_EXTRA_TARGETS += libxlsxwriter_build
QMAKE_EXTRA_TARGETS += libxlsxwriter_conf
PRE_TARGETDEPS += libxlsxwriter_build
