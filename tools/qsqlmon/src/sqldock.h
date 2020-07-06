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
	SqlDock(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

public:
	SqlTextEdit* sqlTextEdit();
public:
	Ui::SqlWidget ui;
};

#endif
