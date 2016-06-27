#include "dbfsattrs.h"

#include <QStringBuilder>

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

QChar DbFsAttrs::typeChar() const
{
	if(type() == Dir)
		return 'd';
	if(type() == File)
		return 'f';
	return QChar();
}

QString DbFsAttrs::typeName() const
{
	if(type() == Dir)
		return QStringLiteral("DIR");
	if(type() == File)
		return QStringLiteral("FILE");
	return QStringLiteral("INVALID");
}

QString DbFsAttrs::toString() const
{
	QString ret;
	if(isNull())
		ret += "NULL - ";
	else if(type() == Dir && inode() == 0 && pinode() == 0)
		ret += "ROOT - ";
	ret += "inode: " + QString::number(inode()) + ", ";
	ret += "pinode: " + QString::number(pinode()) + ", ";
	ret += "type: " % typeName() % ", ";
	ret += "name: " + name();
	return ret;
}

