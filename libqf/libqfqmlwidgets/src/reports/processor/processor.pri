HEADERS +=                            \
	$$PWD/reportpainter.h       \
	$$PWD/reportprocessor.h    \
	$$PWD/reportitem.h   \
#	$$PWD/reportprocessorcontext.h  \
    $$PWD/reportdocument.h \
    $$PWD/reportitemframe.h \
    $$PWD/reportitemreport.h \
    $$PWD/reportitemrepeater.h \
    $$PWD/reportitempara.h \
    $$PWD/reportitemimage.h

SOURCES +=                            \
	$$PWD/reportpainter.cpp       \
	$$PWD/reportprocessor.cpp    \
	$$PWD/reportitem.cpp   \
	$$PWD/reportitem_html.cpp   \
#	$$PWD/reportprocessorcontext.cpp  \
    $$PWD/reportdocument.cpp \
    $$PWD/reportitemframe.cpp \
    $$PWD/reportitemreport.cpp \
    $$PWD/reportitemrepeater.cpp \
    $$PWD/reportitempara.cpp \
    $$PWD/reportitemimage.cpp

include ($$PWD/style/style.pri)
