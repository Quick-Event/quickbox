#include "tableview.h"

using namespace qf::qmlwidgets;

TableView::TableView(QWidget *parent) :
	Super(parent)
{
}

qf::core::model::SqlQueryTableModel *TableView::sqlModel() const
{
	return qobject_cast<qf::core::model::SqlQueryTableModel*>(Super::model());
}

void TableView::setSqlModel(qf::core::model::SqlQueryTableModel *m)
{
	qf::core::model::SqlQueryTableModel *old_m = sqlModel();
	if (old_m != m) {
		Super::setModel(m);
		emit modelChanged();
	}
}
