#include "dbenumcache.h"

#include "query.h"

#include "../core/utils.h"
#include "../core/log.h"

#include <QSqlError>

using namespace qf::core::sql;

//============================================================
//                              DbEnumCache
//============================================================
int DbEnumCache::EnumList::indexOf(const QString& group_id) const
{
	for(int ix=0; ix<count(); ix++) {
		DbEnum en = value(ix);
		if(en.groupId() == group_id) return ix;
	}
	return -1;
}

DbEnum DbEnumCache::EnumList::valueForId(const QString& group_id) const
{
	int ix = indexOf(group_id);
	if(ix >= 0) return value(ix);
	//QF_EXCEPTION(QString("group_id"));
	return DbEnum();
}

void DbEnumCache::clear(const QString & group_name)
{
	if(group_name.isEmpty()) m_enumCache.clear();
	else m_enumCache.remove(group_name);
}

void DbEnumCache::reload(const QString & group_name)
{
	clear(group_name);
	Query q(m_connectionName);
	if(q.exec("SELECT * FROM enumz WHERE groupName=" QF_SARG(group_name) " ORDER BY pos")) {
		while(q.next()) {
			DbEnum en(q);
			m_enumCache[group_name] << en;
		}
	}
	else {
		qfError() << q.lastError().text();
	}
}

void DbEnumCache::ensure(const QString & group_name)
{
	if(!m_enumCache.contains(group_name))
		reload(group_name);
}

DbEnumCache& DbEnumCache::instance(const QString &connection_name)
{
	static QMap<QString, DbEnumCache> instances;
	if(!instances.contains(connection_name))
		instances[connection_name].setConnectionName(connection_name);
	return instances[connection_name];
}

DbEnumCache::EnumList DbEnumCache::dbEnumsForGroup(const QString & group_name)
{
	ensure(group_name);
	return m_enumCache.value(group_name);
}

DbEnum DbEnumCache::dbEnum(const QString & group_name, const QString & group_id)
{
	EnumList enmlst = dbEnumsForGroup(group_name);
	//QF_EXCEPTION(QString("group_id"));
	return enmlst.valueForId(group_id);
}

