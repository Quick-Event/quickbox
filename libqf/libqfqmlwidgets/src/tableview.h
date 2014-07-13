#ifndef QF_QMLWIDGETS_TABLEVIEW_H
#define QF_QMLWIDGETS_TABLEVIEW_H

#include "qmlwidgetsglobal.h"

#include <qf/core/model/sqlquerytablemodel.h>
#include <qf/core/utils/table.h>

#include <QTableView>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT TableView : public QTableView
{
	Q_OBJECT
	Q_PROPERTY(qf::core::model::TableModel* model READ tableModel WRITE setTableModel NOTIFY modelChanged)
private:
	typedef QTableView Super;
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
protected:
	QString m_seekString;
};

}}

#endif // QF_QMLWIDGETS_TABLEVIEW_H
