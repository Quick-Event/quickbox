#include "tablelocker.h"

#include "query.h"

#include "../core/log.h"

#include <QSqlError>

using namespace qf::core::sql;

TableLocker::TableLocker(const Connection &conn, const QString &table_name, const QString &lock_type)
	: Transaction(conn), m_tableName(table_name), m_lockType(lock_type)
{
	lock();
}

TableLocker::~TableLocker()
{
}

void TableLocker::lock()
{
	Query q(m_connection);
	QString qs = "LOCK TABLE " + m_tableName + " IN " + m_lockType + " MODE";
	qfInfo() << qs;
	bool ok = q.exec(qs);
	if(!ok) {
		qfError() << "LOCK table error -" << q.lastError().text();
	}
}
