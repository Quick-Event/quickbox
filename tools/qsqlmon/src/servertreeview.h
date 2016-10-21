
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef SERVERTREEVIEW_H
#define SERVERTREEVIEW_H

//#include <qfglobal.h>

#include <QTreeView>



class  ServerTreeView : public QTreeView
{
	Q_OBJECT
public:
	ServerTreeView(QWidget *parent = NULL);
	virtual ~ServerTreeView();

	virtual void keyPressEvent(QKeyEvent *event);
};

#endif // SERVERTREEVIEW_H

