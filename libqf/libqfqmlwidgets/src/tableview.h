#ifndef QF_QMLWIDGETS_TABLEVIEW_H
#define QF_QMLWIDGETS_TABLEVIEW_H

#include "qmlwidgetsglobal.h"

#include <qf/core/model/sqlquerytablemodel.h>

#include <QTableView>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT TableView : public QTableView
{
	Q_OBJECT
	Q_PROPERTY(qf::core::model::SqlQueryTableModel* model READ sqlModel WRITE setSqlModel NOTIFY modelChanged)
private:
	typedef QTableView Super;
public:
explicit TableView(QWidget *parent = 0);
public:
	qf::core::model::SqlQueryTableModel* sqlModel() const;
	void setSqlModel(qf::core::model::SqlQueryTableModel* m);
	Q_SIGNAL void modelChanged();
};

}}

#endif // QF_QMLWIDGETS_TABLEVIEW_H
