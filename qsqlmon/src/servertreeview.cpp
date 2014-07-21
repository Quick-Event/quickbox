
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
#include "servertreeview.h"

#include <QKeyEvent>

#include <qflogcust.h>

ServerTreeView::ServerTreeView(QWidget *parent)
	: QFTreeView(parent)
{
}

ServerTreeView::~ServerTreeView()
{
}
		
void ServerTreeView::keyPressEvent(QKeyEvent *e)
{
	qfTrash() << QF_FUNC_NAME;
	bool modified = (e->modifiers() != Qt::NoModifier && e->modifiers() != Qt::KeypadModifier);
	bool key_enter = (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter);
	//bool alpha_num = (e->key() >= 0x20 && e->key() < 0x80);
	if(!modified && key_enter) {
		QModelIndex ix = currentIndex();
		emit doubleClicked(ix);
		if(model()->rowCount(ix)) {
			setExpanded(ix, true);
		}
		e->ignore();
		return;
	}
	QTreeView::keyPressEvent(e);
}

