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
		QVariant v = Super::data(index, Qt::EditRole);
		int type = v.userType();
		if(type == qMetaTypeId<TimeMs>()) {
			TimeMs t = v.value<TimeMs>();
			return t.toString();
		}
	}
	else if(role == SortRole) {
		QVariant v = Super::data(index, Qt::EditRole);
		int type = v.userType();
		if(type == qMetaTypeId<TimeMs>()) {
			TimeMs t = v.value<TimeMs>();
			return t.msec();
		}
		return Super::data(index, role);
	}
	else if(role == Qt::TextAlignmentRole) {
		QVariant v = Super::data(index, Qt::EditRole);
		if(v.userType() == qMetaTypeId<TimeMs>()) {
			return Qt::AlignRight;
		}
	}
	return Super::data(index, role);
}

QVariant SqlTableModel::rawValueToEdit(int column_index, const QVariant &val) const
{
	QVariant ret = val;
	int type = columnType(column_index);
	if(type == qMetaTypeId<TimeMs>()) {
		TimeMs t;
		if(!ret.isNull()) {
			t = TimeMs(ret.toInt());
		}
		ret = QVariant::fromValue(t);
	}
	return ret;
}

QVariant SqlTableModel::editValueToRaw(int column_index, const QVariant &val) const
{
	QVariant ret = val;
	int type = columnType(column_index);
	if(type == qMetaTypeId<TimeMs>()) {
		TimeMs t = val.value<TimeMs>();
		ret = t.msec();
	}
	return ret;
}
