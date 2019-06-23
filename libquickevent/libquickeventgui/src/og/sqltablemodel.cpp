#include "sqltablemodel.h"

#include <quickevent/core/si/siid.h>
#include <quickevent/core/og/timems.h>

#include <qf/core/log.h>

#include <QMetaType>

namespace quickevent {
namespace gui {
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
		if(type == qMetaTypeId<core::og::LapTimeMs>()) {
			core::og::LapTimeMs t = v.value<core::og::LapTimeMs>();
			return t.toString();
		}
		else if(type == qMetaTypeId<core::si::SiId>()) {
			int id = (int)v.value<core::si::SiId>();
			if(id == 0)
				return QString();// QStringLiteral("null");
			return id;
		}
	}
	else if(role == SortRole) {
		QVariant v = Super::data(index, Qt::EditRole);
		int type = v.userType();
		if(type == qMetaTypeId<core::og::LapTimeMs>()) {
			core::og::LapTimeMs t = v.value<core::og::LapTimeMs>();
			return t.msec();
		}
		return Super::data(index, role);
	}
	else if(role == Qt::TextAlignmentRole) {
		QVariant v = Super::data(index, Qt::EditRole);
		if(v.userType() == qMetaTypeId<core::og::LapTimeMs>()) {
			return Qt::AlignRight;
		}
	}
	return Super::data(index, role);
}

QVariant SqlTableModel::rawValueToEdit(int column_index, const QVariant &val) const
{
	QVariant ret = val;
	int type = columnType(column_index);
	if(type == qMetaTypeId<core::og::LapTimeMs>()) {
		core::og::LapTimeMs t;
		if(!ret.isNull()) {
			t = core::og::LapTimeMs(ret.toInt());
		}
		ret = QVariant::fromValue(t);
	}
	else if(type == qMetaTypeId<core::si::SiId>()) {
		core::si::SiId id(val.toInt());
		ret = QVariant::fromValue(id);
	}
	return ret;
}

QVariant SqlTableModel::editValueToRaw(int column_index, const QVariant &val) const
{
	QVariant ret = val;
	int type = columnType(column_index);
	if(type == qMetaTypeId<core::og::LapTimeMs>()) {
		core::og::LapTimeMs t = val.value<core::og::LapTimeMs>();
		ret = t.isValid()? t.msec(): QVariant(QVariant::Int);
	}
	else if(type == qMetaTypeId<core::si::SiId>()) {
		auto id = (int)val.value<core::si::SiId>();
		if(id == 0)
			ret = QVariant(QVariant::Int);
		else
			ret = id;
	}
	//qfInfo() << val << ret;
	return ret;
}

}}}
