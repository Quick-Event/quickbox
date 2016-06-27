#include "sqltablemodel.h"
#include "timems.h"

#include <quickevent/og/siid.h>

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
		else if(type == qMetaTypeId<SiId>()) {
			int id = (int)v.value<SiId>();
			if(id == 0)
				return QString();// QStringLiteral("null");
			return id;
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
	else if(type == qMetaTypeId<SiId>()) {
		quickevent::og::SiId id(val.toInt());
		ret = QVariant::fromValue(id);
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
	else if(type == qMetaTypeId<SiId>()) {
		auto id = (int)val.value<SiId>();
		if(id == 0)
			ret = QVariant(QVariant::Int);
		else
			ret = id;
	}
	//qfInfo() << val << ret;
	return ret;
}
