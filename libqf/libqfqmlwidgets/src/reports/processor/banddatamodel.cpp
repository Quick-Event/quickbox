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
	qfLogFuncFrame() << this;
}

BandDataModel::~BandDataModel()
{
	qfLogFuncFrame() << this;
}

QVariant BandDataModel::table(int row_no, const QString &table_name)
{
	Q_UNUSED(table_name)
	Q_UNUSED(row_no)
	return QVariant();
}

BandDataModel* BandDataModel::createFromData(const QVariant &data, QObject *parent)
{
	BandDataModel *ret = nullptr;
	// only tree table is supported currently
	// so every data is tree table itself or treetable data for now
	qfu::TreeTable tt;
	if(data.userType() == qMetaTypeId<qfu::TreeTable>()) {
		tt = data.value<qfu::TreeTable>();
	}
	else {
		tt = qfu::TreeTable(data);
	}
	TreeTableBandDataModel *m = new TreeTableBandDataModel(parent);
	m->setTreeTable(tt);
	ret = m;
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
	Q_UNUSED(role)
	QVariant ret;
	qfu::TreeTable ttr = treeTable();
	ret = ttr.value(key);
	//qfWarning() << key << "->" << ret.typeName();
	//qfWarning() << ttr.toString();
	return ret;
}

QVariant TreeTableBandDataModel::headerData(int col_no, BandDataModel::DataRole role)
{
	if(role == Qt::DisplayRole) {
		return treeTable().column(col_no).header();
	}
	else if(role == Qt::SizeHintRole) {
		return treeTable().column(col_no).width();
	}
	return QVariant();
}

QVariant TreeTableBandDataModel::dataByIndex(int row_no, int col_no, BandDataModel::DataRole role)
{
	if(role == Qt::DisplayRole) {
		return treeTable().row(row_no).value(col_no);
	}
	return QVariant();
}

QVariant TreeTableBandDataModel::dataByName(int row_no, const QString &col_name, BandDataModel::DataRole role)
{
	if(role == Qt::DisplayRole) {
		return treeTable().row(row_no).value(col_name);
	}
	return QVariant();
}

QVariant TreeTableBandDataModel::table(int row_no, const QString &table_name)
{
	if(table_name.isEmpty())
		return treeTable().row(row_no).table().toVariant();
	return treeTable().row(row_no).table(table_name).toVariant();
}

QString TreeTableBandDataModel::dump() const
{
	return treeTable().toString();
}
const qf::core::utils::TreeTable& TreeTableBandDataModel::treeTable() const
{
	return m_treeTable;
}

void TreeTableBandDataModel::setTreeTable(const qf::core::utils::TreeTable &tree_table)
{
	m_treeTable = tree_table;
	setDataValid(true);
}

