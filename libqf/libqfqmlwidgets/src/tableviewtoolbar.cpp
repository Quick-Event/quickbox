#include "tableviewtoolbar.h"
#include "tableview.h"
#include "action.h"

#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

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
		connect(this, &TableViewToolBar::filterStringChanged, table_view, &TableView::filterByString);
		connect(table_view, &TableView::filterDialogRequest, this, &TableViewToolBar::onFilterDialogRequest);
	}
}

void TableViewToolBar::addPendingActions()
{
	QList<QAction*> lst;
	for(auto a : m_pendingActions)
		lst << a;
	addActions(lst);
	QLabel *lbl = new QLabel(tr("Filter"));
	lbl->setPixmap(QPixmap(":/qf/qmlwidgets/images/find.png"));
	addWidget(lbl);
	m_filterCombo = new QComboBox();
	m_filterCombo->setEditable(true);
	connect(m_filterCombo, &QComboBox::editTextChanged, this, &TableViewToolBar::emitFilterStringChanged);
	connect(m_filterCombo, SIGNAL(activated(QString)), this, SLOT(emitFilterStringChanged(QString)));
	addWidget(m_filterCombo);
}

void TableViewToolBar::emitFilterStringChanged(const QString &s)
{
	qfLogFuncFrame() << s;
	emit filterStringChanged(s);
	m_filterCombo->setFocus();
}

void TableViewToolBar::onFilterDialogRequest()
{
	m_filterCombo->lineEdit()->selectAll();
	m_filterCombo->setFocus();
}
