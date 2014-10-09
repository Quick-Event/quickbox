HEADERS +=                            \
	$$PWD/reportpainter.h       \
	$$PWD/reportprocessor.h    \
	$$PWD/reportitem.h   \
#	$$PWD/reportprocessorcontext.h  \
    $$PWD/reportdocument.h \

SOURCES +=                            \
	$$PWD/reportpainter.cpp       \
	$$PWD/reportprocessor.cpp    \
	$$PWD/reportitem.cpp   \
	$$PWD/reportitem_html.cpp   \
#	$$PWD/reportprocessorcontext.cpp  \
    $$PWD/reportdocument.cpp \

include ($$PWD/style/style.pri)
