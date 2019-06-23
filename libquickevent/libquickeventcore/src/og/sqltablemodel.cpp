#include "sqltablemodel.h"
#include "timems.h"
#include "../si/siid.h"

#include <qf/core/log.h>

#include <QMetaType>

namespace quickevent {
namespace core {
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
		if(type == qMetaTypeId<LapTimeMs>()) {
			LapTimeMs t = v.value<LapTimeMs>();
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
		if(type == qMetaTypeId<LapTimeMs>()) {
			LapTimeMs t = v.value<LapTimeMs>();
			return t.msec();
		}
		return Super::data(index, role);
	}
	else if(role == Qt::TextAlignmentRole) {
		QVariant v = Super::data(index, Qt::EditRole);
		if(v.userType() == qMetaTypeId<LapTimeMs>()) {
			return Qt::AlignRight;
		}
	}
	return Super::data(index, role);
}

QVariant SqlTableModel::rawValueToEdit(int column_index, const QVariant &val) const
{
	QVariant ret = val;
	int type = columnType(column_index);
	if(type == qMetaTypeId<LapTimeMs>()) {
		LapTimeMs t;
		if(!ret.isNull()) {
			t = LapTimeMs(ret.toInt());
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
	if(type == qMetaTypeId<LapTimeMs>()) {
		LapTimeMs t = val.value<LapTimeMs>();
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

}}}
