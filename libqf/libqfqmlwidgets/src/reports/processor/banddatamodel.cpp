#include "banddatamodel.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/string.h>

#include <qf/core/utils/timescope.h>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

//=======================================================
//                   BandDataModel
//=======================================================
BandDataModel::BandDataModel(QObject *parent) :
	Super(parent)
{
}

QVariant BandDataModel::table(int row_no, const QString &table_name)
{
	Q_UNUSED(table_name);
	Q_UNUSED(row_no);
	return QVariant();
}

BandDataModel* BandDataModel::createFromData(const QVariant &data, QObject *parent)
{
	BandDataModel *ret = nullptr;
	if(data.userType() == qMetaTypeId<qfu::TreeTable>()) {
		TreeTableBandDataModel *m = new TreeTableBandDataModel(parent);
		qfu::TreeTable tt = data.value<qfu::TreeTable>();
		m->setTreeTable(tt);
		ret = m;
	}
	return ret;
}

//=======================================================
//                   TreeTableBandDataModel
//=======================================================
TreeTableBandDataModel::TreeTableBandDataModel(QObject *parent)
	: Super(parent)
{

}

int TreeTableBandDataModel::rowCount()
{
	QF_TIME_SCOPE("TreeTableBandDataModel::rowCount");
	return treeTable().rowCount();
}

int TreeTableBandDataModel::columnCount()
{
	return treeTable().columnCount();
}

QVariant TreeTableBandDataModel::tableData(const QString &key, BandDataModel::DataRole role)
{
	Q_UNUSED(role);
	QVariant ret;
	qfu::TreeTable ttr = treeTable();
	ret = ttr.value(key);
	return ret;
}

QVariant TreeTableBandDataModel::headerData(int col_no, BandDataModel::DataRole role)
{
	QVariant ret;
	qfu::TreeTableColumns cols = treeTable().columns();
	qfu::TreeTableColumn col = cols.column(col_no);
	if(col.isValid()) {
		if(role == Qt::DisplayRole) {
			ret = col.header();
		}
		else if(role == Qt::SizeHintRole) {
			ret = col.width();
		}
	}
	return ret;
}

QVariant TreeTableBandDataModel::data(int row_no, int col_no, BandDataModel::DataRole role)
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

QVariant TreeTableBandDataModel::data(int row_no, const QString &col_name, BandDataModel::DataRole role)
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

QVariant TreeTableBandDataModel::table(int row_no, const QString &table_name)
{
	QVariant ret;
	qfu::TreeTableRow ttr = treeTable().row(row_no);
	if(!ttr.isNull()) {
		for(int i=0; i<ttr.tablesCount(); i++) {
			qfu::TreeTable tt = ttr.table(i);
			if(table_name.isEmpty() || tt.name() == table_name) {
				ret = QVariant::fromValue(tt);
				break;
			}
		}
	}
	return ret;
}

QString TreeTableBandDataModel::dump() const
{
	return treeTable().toString();
}
