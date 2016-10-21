#ifndef SQLDOCK_H
#define SQLDOCK_H

#include <QDockWidget>

#include "ui_sqlwidget.h"

class QAction;
class QActionGroup;
class QMenu;

class SqlDock : public QDockWidget
{
	Q_OBJECT
public:
	SqlDock(QWidget *parent = 0, Qt::WindowFlags flags = 0);

public:
	SqlTextEdit* sqlTextEdit();
public:
	Ui::SqlWidget ui;
};

#endif
