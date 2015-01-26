#include "dbfsattrs.h"

using namespace qf::core::sql;

const DbFsAttrs &DbFsAttrs::sharedNull()
{
	static DbFsAttrs n = DbFsAttrs(SharedDummyHelper());
	return n;
}

DbFsAttrs::DbFsAttrs(DbFsAttrs::SharedDummyHelper)
{
	d = new Data();
}

DbFsAttrs::DbFsAttrs()
{
	*this = sharedNull();
}

DbFsAttrs::DbFsAttrs(NodeType t, bool deleted)
{
	d = new Data(t, deleted);
}

