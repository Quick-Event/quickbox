
HEADERS  +=     \
	$$PWD/crc529.h    \

SOURCES +=     \
	$$PWD/sidevicedriver.cpp     \
	$$PWD/commport.cpp    \
	$$PWD/crc529.c    \

win32: {
HEADERS  +=     \
	$$PWD/commport_win.h  \

SOURCES +=     \
	$$PWD/commport_win.cpp  \

}

unix: {
HEADERS  +=     \
	$$PWD/commport_unix.h   \

SOURCES +=     \
	$$PWD/commport_unix.cpp   \

}
