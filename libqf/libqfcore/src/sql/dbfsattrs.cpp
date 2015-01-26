#include "dbfsattrs.h"

using namespace qf::core::sql;

const DbFsAttrs &DbFsAttrs::sharedNull()
{
	static DbFsAttrs n = DbFsAttrs(SharedDummyHelper());
	return n;
}

DbFsAttrs::DbFsAttrs(DbFsAttrs::SharedDummyHelper)
{
	d = new Data(Invalid);
}

DbFsAttrs::DbFsAttrs()
{
	*this = sharedNull();
}

DbFsAttrs::DbFsAttrs(DbFsAttrs::NodeType t)
{
	d = new Data(t);
}

QString DbFsAttrs::typeString() const
{
	if(type() == Dir)
		return QStringLiteral("d");
	if(type() == File)
		return QStringLiteral("f");
	return QString();
}

QString DbFsAttrs::toString() const
{
	QString ret;
	if(isNull())
		ret += "NULL - ";
	else if(type() == Dir && inode() == 0 && pinode() == 0)
		ret += "ROOT - ";
	ret += "id: " + QString::number(id()) + ", ";
	ret += "inode: " + QString::number(inode()) + ", ";
	ret += "pinode: " + QString::number(pinode()) + ", ";
	ret += "type: " + typeString() + ", ";
	ret += "name: " + name() + ", ";
	ret += "deleted: ";
	ret += isDeleted()? "Y": "N";
	return ret;
}

