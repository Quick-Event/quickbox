contains(QMAKE_LFLAGS, -m32) {
    message( "32 bit build" )
    message( To crosscompile on Ubuntu-64 add symlink libGL.so->libGl.so.1 in directory /usr/lib/i386-linux-gnu/mesa )
    # libGL.so is mot really needed for libqfcore build but I don't know, how to remove -lGL from Makefile
    LIBS += -L/usr/lib/i386-linux-gnu/mesa/ \
}
