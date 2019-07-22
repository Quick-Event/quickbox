#ifndef QF_QMLWIDGETS_TABLEVIEWTOOLBAR_H
#define QF_QMLWIDGETS_TABLEVIEWTOOLBAR_H

#include "qmlwidgetsglobal.h"

#include <QToolBar>

class QComboBox;

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
	explicit TableViewToolBar(QWidget *parent = nullptr);
	~TableViewToolBar() Q_DECL_OVERRIDE;
public:
	void setTableView(TableView *table_view);
	qf::qmlwidgets::TableView* tableView() const { return m_tableView; }
signals:
	void filterStringChanged(const QString &s);
private:
	Q_SLOT void addPendingActions();
	Q_SLOT void emitFilterStringChanged(const QString &s);
	Q_SLOT void onFilterDialogRequest();
private:
	qf::qmlwidgets::TableView* m_tableView = nullptr;
	QList<Action*> m_pendingActions;
	QComboBox *m_filterCombo = nullptr;
};

}}

#endif // QF_QMLWIDGETS_TABLEVIEWTOOLBAR_H
