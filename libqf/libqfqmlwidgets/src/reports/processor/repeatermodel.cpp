#include "repeatermodel.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/string.h>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

//=======================================================
//                   RepeaterModel
//=======================================================
RepeaterModel::RepeaterModel(QObject *parent) :
	Super(parent)
{
}

QVariant RepeaterModel::table(int row_no, const QString &table_name)
{
	Q_UNUSED(table_name);
	Q_UNUSED(row_no);
	return QVariant();
}

RepeaterModel* RepeaterModel::createFromData(const QVariant &data, QObject *parent)
{
	RepeaterModel *ret = nullptr;
	if(data.userType() == qMetaTypeId<qfu::TreeTable>()) {
		RepeaterModelTreeTable *m = new RepeaterModelTreeTable(parent);
		qfu::TreeTable tt = data.value<qfu::TreeTable>();
		m->setTreeTable(tt);
		ret = m;
	}
	return ret;
}

//=======================================================
//                   RepeaterModelTreeTable
//=======================================================
RepeaterModelTreeTable::RepeaterModelTreeTable(QObject *parent)
	: Super(parent)
{

}

int RepeaterModelTreeTable::rowCount()
{
	return treeTable().rowCount();
}

int RepeaterModelTreeTable::columnCount()
{
	return treeTable().columnCount();
}

QVariant RepeaterModelTreeTable::headerData(int col_no, RepeaterModel::DataRole role)
{
	QVariant ret;
	qfu::TreeTableColumns cols = treeTable().columns();
	qfu::TreeTableColumn col = cols.column(col_no);
	if(col.isValid()) {
		if(role == Qt::DisplayRole) {
			ret = col.header();
		}
	}
	return ret;
}

QVariant RepeaterModelTreeTable::data(int row_no, int col_no, RepeaterModel::DataRole role)
{
	QVariant ret;
	qfu::TreeTableRow ttr = treeTable().row(row_no);
	if(!ttr.isNull()) {
		if(role == Qt::DisplayRole) {
			ret = ttr.value(col_no);
		}
	}
	return ret;
}

QVariant RepeaterModelTreeTable::data(int row_no, const QString &col_name, RepeaterModel::DataRole role)
{
	QVariant ret;
	qfu::TreeTableRow ttr = treeTable().row(row_no);
	if(!ttr.isNull()) {
		if(role == Qt::DisplayRole) {
			ret = ttr.value(col_name);
		}
	}
	return ret;
}

QVariant RepeaterModelTreeTable::table(int row_no, const QString &table_name)
{
	QVariant ret;
	qfu::TreeTableRow ttr = treeTable().row(row_no);
	if(!ttr.isNull()) {
		for(int i=0; i<ttr.tablesCount(); i++) {
			qfu::TreeTable tt = ttr.table(i);
			if(table_name.isEmpty() || tt.name() == table_name) {
				ret = QVariant::fromValue(tt);
			}
		}
	}
	return ret;
}
