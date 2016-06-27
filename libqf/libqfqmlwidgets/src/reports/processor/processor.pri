HEADERS +=                            \
	$$PWD/reportpainter.h       \
	$$PWD/reportprocessor.h    \
	$$PWD/reportitem.h   \
#	$$PWD/reportprocessorcontext.h  \
    $$PWD/reportdocument.h \
    $$PWD/reportitemframe.h \
    $$PWD/reportitemreport.h \
    $$PWD/reportitempara.h \
    $$PWD/reportitemimage.h \
    $$PWD/banddatamodel.h \
    $$PWD/reportitemband.h \
    $$PWD/reportitemdetail.h \
    $$PWD/reportitembreak.h

SOURCES +=                            \
	$$PWD/reportpainter.cpp       \
	$$PWD/reportprocessor.cpp    \
	$$PWD/reportitem.cpp   \
	$$PWD/reportitem_html.cpp   \
#	$$PWD/reportprocessorcontext.cpp  \
    $$PWD/reportdocument.cpp \
    $$PWD/reportitemframe.cpp \
    $$PWD/reportitemreport.cpp \
    $$PWD/reportitempara.cpp \
    $$PWD/reportitemimage.cpp \
    $$PWD/banddatamodel.cpp \
    $$PWD/reportitemband.cpp \
    $$PWD/reportitemdetail.cpp \
    $$PWD/reportitembreak.cpp

include ($$PWD/style/style.pri)
