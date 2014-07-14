#ifndef QF_QMLWIDGETS_TABLEVIEW_H
#define QF_QMLWIDGETS_TABLEVIEW_H

#include "qmlwidgetsglobal.h"

#include <qf/core/model/sqlquerytablemodel.h>
#include <qf/core/utils/table.h>

#include <QTableView>

namespace qf {
namespace qmlwidgets {

class Action;

class QFQMLWIDGETS_DECL_EXPORT TableView : public QTableView
{
	Q_OBJECT
	Q_PROPERTY(qf::core::model::TableModel* model READ tableModel WRITE setTableModel NOTIFY modelChanged)
private:
	typedef QTableView Super;
public:
	enum ActionGroup {ViewActions = 1, BlobActions = 2, SetValueActions = 4, RowActions = 8, CellActions = 16, SizeActions = 32,
					   CalculateActions = 64, ExportActions = 128, ImportActions = 256, SortActions = 512, SelectActions = 1024,
					   FilterActions = 2048, PasteActions = 4096, AllActions = 65535};
public:
	explicit TableView(QWidget *parent = 0);
public:
	qf::core::model::TableModel* tableModel() const;
	void setTableModel(qf::core::model::TableModel* m);
	Q_SIGNAL void modelChanged();
private:
	Q_SIGNAL void searchStringChanged(const QString &str);
	qf::core::utils::Table::SortDef seekSortDefinition() const;
	int seekColumn() const;
	void seek(const QString &prefix_str);
protected:
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
	virtual void createActions();
	QList<Action*> contextMenuActionsForGroups(int action_groups = AllActions);
protected:
	QString m_seekString;
	QMap<QString, Action*> m_actions;
	QMap<ActionGroup, Action*> m_separatorsForGroup;
	QMap<int, QStringList> m_actionGroups;
};

}}

#endif // QF_QMLWIDGETS_TABLEVIEW_H
