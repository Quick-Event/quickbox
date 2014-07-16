#include "tableviewtoolbar.h"
#include "tableview.h"
#include "action.h"

//namespace qfc = qf::core;
//namespace qfu = qf::core::utils;
using namespace qf::qmlwidgets;

TableViewToolBar::TableViewToolBar(QWidget *parent) :
	QToolBar(parent)
{
}

TableViewToolBar::~TableViewToolBar()
{
}

void TableViewToolBar::setTableView(TableView *table_view)
{
	if(table_view) {
		m_pendingActions = table_view->toolBarActions();
		/// cannot add actions here from QML context because of bug in Qt5.3.1
		QMetaObject::invokeMethod(this, "addPendingActions", Qt::QueuedConnection);
	}
}

void TableViewToolBar::addPendingActions()
{
	QList<QAction*> lst;
	for(auto a : m_pendingActions)
		lst << a;
	addActions(lst);
}
