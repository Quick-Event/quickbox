#include "dbenumcache.h"

#include "query.h"

#include "../core/utils.h"
#include "../core/log.h"

#include <QSqlError>

using namespace qf::core::sql;

//============================================================
//                              DbEnumCache
//============================================================

namespace {

std::map< QString, DbEnumCache > instances;

}

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

DbEnumCache::DbEnumCache(const QString &connection_name)
 : m_connectionName(connection_name)
{
	if(m_connectionName.isEmpty()) {
		static QString cn = QString::fromLatin1(QSqlDatabase::defaultConnection);
		m_connectionName = cn;
	}
	qfInfo() << "creating new DbEnumCache for connection name:" << m_connectionName;
}

DbEnumCache::~DbEnumCache()
{
	// next line can cause seg fault on app exit because logDevice() can be destroyied before
	//qfInfo() << "destroying DbEnumCache for connection name:" << m_connectionName;
}

void DbEnumCache::clear(const QString & group_name)
{
	if(group_name.isEmpty())
		m_enumsForGroup.clear();
	else
		m_enumsForGroup.remove(group_name);
}

void DbEnumCache::reload(const QString & group_name)
{
	if(m_connectionName.isEmpty()) {
		qfError() << "Attempt to load invalid DbEnumCache";
		return;
	}
	clear(group_name);
	EnumList &enumz = m_enumsForGroup[group_name];
	Query q(m_connectionName);
	if(q.exec("SELECT * FROM enumz WHERE groupName=" QF_SARG(group_name) " ORDER BY pos", qf::core::Exception::Throw)) {
		while(q.next()) {
			DbEnum en(q);
			enumz << en;
		}
	}
	else {
		qfError() << q.lastError().text();
	}
}

void DbEnumCache::ensure(const QString & group_name)
{
	if(!m_enumsForGroup.contains(group_name)) {
		reload(group_name);
	}
}

DbEnumCache& DbEnumCache::instanceForConnection(const QString &connection_name)
{
	QString cn = connection_name;
	if(cn.isEmpty())
		cn = QSqlDatabase::defaultConnection;
	if(instances.count(cn) == 0) {
		instances.emplace(cn, DbEnumCache{cn});
	}
	return instances[cn];
}

DbEnumCache::EnumList DbEnumCache::dbEnumsForGroup(const QString & group_name)
{
	ensure(group_name);
	return m_enumsForGroup.value(group_name);
}

DbEnum DbEnumCache::dbEnum(const QString & group_name, const QString & group_id)
{
	EnumList enmlst = dbEnumsForGroup(group_name);
	//QF_EXCEPTION(QString("group_id"));
	return enmlst.valueForId(group_id);
}

