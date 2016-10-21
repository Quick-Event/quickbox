#include "sqltablemodel.h"
#include "timems.h"

#include <quickevent/si/siid.h>

#include <qf/core/log.h>

#include <QMetaType>

namespace quickevent {
namespace og {

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
		else if(type == qMetaTypeId<si::SiId>()) {
			int id = (int)v.value<si::SiId>();
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
	else if(type == qMetaTypeId<si::SiId>()) {
		si::SiId id(val.toInt());
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
		ret = t.isValid()? t.msec(): QVariant(QVariant::Int);
	}
	else if(type == qMetaTypeId<si::SiId>()) {
		auto id = (int)val.value<si::SiId>();
		if(id == 0)
			ret = QVariant(QVariant::Int);
		else
			ret = id;
	}
	//qfInfo() << val << ret;
	return ret;
}

}}
