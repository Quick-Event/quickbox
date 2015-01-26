include(driver/driver.pri)

HEADERS += \
	$$PWD/qfclassfield.h      \
	$$PWD/mainwindow.h      \
	$$PWD/sqldock.h      \
	$$PWD/servertreedock.h      \
	$$PWD/servertreeitem.h      \
	$$PWD/servertreemodel.h      \
	$$PWD/sqltextedit.h      \
	$$PWD/dlgeditconnection.h      \
	$$PWD/dlgaltertable.h      \
	$$PWD/dlgcolumndef.h      \
	$$PWD/dlgindexdef.h      \
	$$PWD/servertreeview.h     \
	$$PWD/theapp.h    \
	$$PWD/tableviewwidget.h   \
	$$PWD/columnselectorwidget.h  \
	$$PWD/qfsqlsyntaxhighlighter.h \
	$$PWD/qfobjectitemmodel.h \
	$$PWD/qfstatusbar.h \

SOURCES += \
	$$PWD/main.cpp      \
	$$PWD/mainwindow.cpp      \
	$$PWD/sqldock.cpp      \
	$$PWD/servertreedock.cpp      \
	$$PWD/servertreeitem.cpp      \
	$$PWD/servertreemodel.cpp      \
	$$PWD/sqltextedit.cpp      \
	$$PWD/dlgeditconnection.cpp      \
	$$PWD/dlgaltertable.cpp      \
	$$PWD/dlgcolumndef.cpp      \
	$$PWD/dlgindexdef.cpp      \
	$$PWD/servertreeview.cpp     \
	$$PWD/theapp.cpp    \
	$$PWD/tableviewwidget.cpp   \
	$$PWD/columnselectorwidget.cpp  \
	$$PWD/qfsqlsyntaxhighlighter.cpp \
	$$PWD/qfobjectitemmodel.cpp \
	$$PWD/qfstatusbar.cpp \

FORMS += \
	$$PWD/centralwidget.ui      \
	$$PWD/servertreewidget.ui      \
	$$PWD/sqlwidget.ui      \
	$$PWD/dlgeditconnection.ui      \
	$$PWD/dlgaltertable.ui      \
	$$PWD/dlgcolumndef.ui      \
	$$PWD/dlgindexdef.ui      \
	$$PWD/columnselectorwidget.ui  \
	$$PWD/tableviewwidget.ui \

RESOURCES += \
	$$PWD/qsqlmon.qrc

RC_FILE = $$PWD/qsqlmon.rc


