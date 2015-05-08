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
	if(role == Qt::DisplayRole) {
		QVariant v = Super::data(index, RawValueRole);
		int type = v.userType();
		if(type == qMetaTypeId<TimeMs>()) {
			TimeMs t = v.value<TimeMs>();
			return t.toString();
		}
	}
	else if(role == SortRole) {
		QVariant v = Super::data(index, RawValueRole);
		int type = v.userType();
		if(type == qMetaTypeId<TimeMs>()) {
			TimeMs t = v.value<TimeMs>();
			return t.msec();
		}
	}
	else if(role == Qt::TextAlignmentRole) {
		QVariant v = Super::data(index, RawValueRole);
		if(v.userType() == qMetaTypeId<TimeMs>()) {
			return Qt::AlignRight;
		}
	}
	return Super::data(index, role);
}

QVariant SqlTableModel::value(int row_ix, int column_ix) const
{
	//qfLogFuncFrame() << row_ix << column_ix << columnType(column_ix);
	QVariant ret = Super::value(row_ix, column_ix);
	int type = columnType(column_ix);
	if(type == qMetaTypeId<TimeMs>()) {
		TimeMs t;
		if(!ret.isNull()) {
			t = TimeMs(ret.toInt());
		}
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
