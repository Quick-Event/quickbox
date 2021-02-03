#include "sqldock.h"

#include <qaction.h>
#include <qevent.h>
#include <qframe.h>
#include <qmainwindow.h>
#include <qmenu.h>

SqlDock::SqlDock(QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
{
    setObjectName("SQL");
    setWindowTitle(objectName());

    QWidget *w = new QWidget(this);
	ui.setupUi(w);
	//w->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    setWidget(w);
}

SqlTextEdit* SqlDock::sqlTextEdit()
{
	return ui.txtSql;
}
