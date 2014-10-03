HEADERS +=                               \
	$$PWD/reportviewwidget.h              \
	$$PWD/itemvalueeditorwidget.h  \

SOURCES +=                               \
	$$PWD/reportviewwidget.cpp              \
	$$PWD/itemvalueeditorwidget.cpp  \

FORMS +=      \
	$$PWD/itemvalueeditorwidget.ui   \

RESOURCES +=                         \

include($$PWD/printtableviewwidget/printtableviewwidget.pri)
