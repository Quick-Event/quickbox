#include "sqltablemodel.h"
#include "timems.h"

#include <qf/core/log.h>

#include <QMetaType>

using namespace quickevent::og;

SqlTableModel::SqlTableModel(QObject *parent)
	: Super(parent)
{

}

SqlTableModel::~SqlTableModel()
{

}

QVariant SqlTableModel::data(const QModelIndex &index, int role) const
{
	QVariant ret = Super::data(index, role);
	if(role == Qt::DisplayRole) {
		int type = ret.userType();
		if(type == qMetaTypeId<TimeMs>()) {
			TimeMs t = ret.value<TimeMs>();
			ret = t.toString();
		}
	}
	else if(role == Qt::TextAlignmentRole) {
		QVariant v = Super::data(index, RawValueRole);
		if(v.userType() == qMetaTypeId<TimeMs>()) {
			ret = Qt::AlignRight;
		}
	}
	return ret;
}

QVariant SqlTableModel::value(int row_ix, int column_ix) const
{
	//qfLogFuncFrame() << row_ix << column_ix << columnType(column_ix);
	QVariant ret = Super::value(row_ix, column_ix);
	int type = columnType(column_ix);
	if(type == qMetaTypeId<TimeMs>()) {
		TimeMs t(ret.toInt());
		ret = QVariant::fromValue(t);
	}
	return ret;
}

bool SqlTableModel::setValue(int row, int column, const QVariant &val)
{
	int type = val.userType();
	QVariant v = val;
	if(type == qMetaTypeId<TimeMs>()) {
		TimeMs t = val.value<TimeMs>();
		v = t.msec();
	}
	return Super::setValue(row, column, v);
}
/*
int SqlTableModel::ogTimeMsTypeId() const
{
	//qfWarning() << "qMetaTypeId<TimeMs>():" << qMetaTypeId<TimeMs>();
	return qMetaTypeId<TimeMs>();
}
*/
