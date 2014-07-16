#ifndef QF_QMLWIDGETS_TABLEVIEWTOOLBAR_H
#define QF_QMLWIDGETS_TABLEVIEWTOOLBAR_H

#include "qmlwidgetsglobal.h"

#include <QToolBar>

namespace qf {
namespace qmlwidgets {

class TableView;
class Action;

class QFQMLWIDGETS_DECL_EXPORT TableViewToolBar : public QToolBar
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::TableView* tableView READ tableView WRITE setTableView FINAL)
private:
	typedef QToolBar Super;
public:
	explicit TableViewToolBar(QWidget *parent = 0);
	~TableViewToolBar() Q_DECL_OVERRIDE;
public:
	void setTableView(TableView *table_view);
	qf::qmlwidgets::TableView* tableView() const { return m_tableView; }
private:
	Q_SLOT void addPendingActions();
private:
	qf::qmlwidgets::TableView* m_tableView;
	QList<Action*> m_pendingActions;
};

}}

#endif // QF_QMLWIDGETS_TABLEVIEWTOOLBAR_H
