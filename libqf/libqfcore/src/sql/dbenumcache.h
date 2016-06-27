//
// Author: Frantisek Vacek <fanda.vacek@gmail.com>, (C) 2004,2015
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_CORE_SQL_DBENUMCACHE_H
#define QF_CORE_SQL_DBENUMCACHE_H

#include "../core/coreglobal.h"

#include "dbenum.h"

#include <QMap>
#include <QSharedPointer>

namespace qf {
namespace core {
namespace sql {

class QFCORE_DECL_EXPORT DbEnumCache
{
public:
	DbEnumCache(const QString &connection_name = QString());
	virtual ~DbEnumCache();
public:
	struct QFCORE_DECL_EXPORT EnumList : public QList<DbEnum>
	{
		int indexOf(const QString &group_id) const;
		DbEnum valueForId(const QString &group_id) const;
	};
public:
	//bool isValid() const {return !m_connectionName.isEmpty();}
	QString connectionName() const;

	static DbEnumCache &instanceForConnection(const QString &connection_name = QString());

	EnumList dbEnumsForGroup(const QString &group_name);
	DbEnum dbEnum(const QString &group_name, const QString &group_id);

	bool isEmpty() const {return m_enumsForGroup.isEmpty();}
	virtual void reload(const QString &group_name);
	//! delete all groups wen group_name is empty
	void clear(const QString &group_name = QString());
	/// ensure that group_name enums are loaded in the cache
	void ensure(const QString & group_name);
private:
	QString m_connectionName;
	QMap<QString, EnumList> m_enumsForGroup;
};

}}}

#endif // QF_CORE_SQL_DBENUMCACHE_H

