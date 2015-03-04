#include "ogsqltablemodel.h"
#include "ogtimems.h"

#include <qf/core/log.h>

#include <QMetaType>

OGSqlTableModel::OGSqlTableModel(QObject *parent)
	: Super(parent)
{

}

OGSqlTableModel::~OGSqlTableModel()
{

}

QVariant OGSqlTableModel::data(const QModelIndex &index, int role) const
{
	QVariant ret = Super::data(index, role);
	if(role == Qt::DisplayRole) {
		int type = ret.userType();
		if(type == qMetaTypeId<OGTimeMs>()) {
			OGTimeMs t = ret.value<OGTimeMs>();
			ret = t.toString();
		}
	}
	return ret;
}

QVariant OGSqlTableModel::value(int row_ix, int column_ix) const
{
	//qfLogFuncFrame() << row_ix << column_ix << columnType(column_ix);
	QVariant ret = Super::value(row_ix, column_ix);
	int type = columnType(column_ix);
	if(type == qMetaTypeId<OGTimeMs>()) {
		OGTimeMs t(ret.toInt());
		ret = QVariant::fromValue(t);
	}
	return ret;
}

bool OGSqlTableModel::setValue(int row, int column, const QVariant &val)
{
	int type = val.userType();
	QVariant v = val;
	if(type == qMetaTypeId<OGTimeMs>()) {
		OGTimeMs t = val.value<OGTimeMs>();
		v = t.msec();
	}
	return Super::setValue(row, column, v);
}

int OGSqlTableModel::ogTimeMsTypeId() const
{
	//qfWarning() << "qMetaTypeId<OGTimeMs>():" << qMetaTypeId<OGTimeMs>();
	return qMetaTypeId<OGTimeMs>();
}

