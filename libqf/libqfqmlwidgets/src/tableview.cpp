#include "tableview.h"

#include <QHeaderView>

using namespace qf::qmlwidgets;

TableView::TableView(QWidget *parent) :
	Super(parent)
{
	//verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	verticalHeader()->setDefaultSectionSize((int)(fontMetrics().lineSpacing() * 1.3));
	setSortingEnabled(true);
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
