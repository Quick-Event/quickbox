
RESOURCES +=      \
	qsqlmon.qrc

RC_FILE = qsqlmon.rc

#TRANSLATIONS    = qsqlmon_cz.ts

win32:CONFIG(debug, debug|release):CONFIG += console
console: message(CONSOLE)

include(driver/driver.pri)

HEADERS +=      \
	qfclassfield.h      \
	mainwindow.h      \
	sqldock.h      \
	servertreedock.h      \
	servertreeitem.h      \
	servertreemodel.h      \
	sqltextedit.h      \
	dlgeditconnection.h      \
	dlgaltertable.h      \
	dlgcolumndef.h      \
	dlgindexdef.h      \
	servertreeview.h     \
	theapp.h    \
	tableviewwidget.h   \
	columnselectorwidget.h  \
	qfsqlsyntaxhighlighter.h \
	qfobjectitemmodel.h \
	qfsqlcatalog.h \
	qfstatusbar.h \

SOURCES +=      \
	main.cpp      \
	mainwindow.cpp      \
	sqldock.cpp      \
	servertreedock.cpp      \
	servertreeitem.cpp      \
	servertreemodel.cpp      \
	sqltextedit.cpp      \
	dlgeditconnection.cpp      \
	dlgaltertable.cpp      \
	dlgcolumndef.cpp      \
	dlgindexdef.cpp      \
	servertreeview.cpp     \
	theapp.cpp    \
	tableviewwidget.cpp   \
	columnselectorwidget.cpp  \
	qfsqlsyntaxhighlighter.cpp \
	qfobjectitemmodel.cpp \
	qfsqlcatalog.cpp \
	qfstatusbar.cpp \

FORMS +=      \
	centralwidget.ui      \
	servertreewidget.ui      \
	sqlwidget.ui      \
	dlgeditconnection.ui      \
	dlgaltertable.ui      \
	dlgcolumndef.ui      \
	dlgindexdef.ui      \
	columnselectorwidget.ui  \
    $$PWD/tableviewwidget.ui


