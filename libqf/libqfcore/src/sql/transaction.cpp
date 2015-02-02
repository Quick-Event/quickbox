#include "transaction.h"

#include "../core/log.h"

#include <QSqlError>

using namespace qf::core::sql;

Transaction::Transaction(const Connection &conn)
	: m_connection(conn)
{
	m_connection.transaction();
}
Transaction::~Transaction()
{
	if(!m_finished)
		rollback();
}
void Transaction::commit()
{
	bool ok = m_connection.commit();
	qfInfo() << "COMMIT";
	if(!ok) {
		qfError() << "COMMIT transaction error -" << m_connection.lastError().text();
	}
	m_finished = true;
}
void Transaction::rollback()
{
	bool ok = m_connection.rollback();
	qfInfo() << "ROLLBACK";
	if(!ok) {
		qfError() << "ROLLBACK transaction error -" << m_connection.lastError().text();
	}
	m_finished = true;
}
