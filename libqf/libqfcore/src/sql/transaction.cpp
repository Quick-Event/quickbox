#include "transaction.h"

#include "../core/log.h"

#include <QSqlError>

using namespace qf::core::sql;

Transaction::Transaction(const Connection &conn)
	: m_connection(conn)
{
	if(!m_connection.isValid())
		m_connection = Connection::forName();
	if(m_connection.isValid())
		m_connection.transaction();
}
Transaction::~Transaction()
{
	if(!m_finished)
		rollback();
}
void Transaction::commit()
{
	if(!m_connection.isValid()) {
		m_finished = true;
		return;
	}
	bool ok = m_connection.commit();
	qfInfo() << "COMMIT";
	if(!ok) {
		qfError() << "COMMIT transaction error -" << m_connection.lastError().text();
	}
	m_finished = true;
}
void Transaction::rollback()
{
	if(!m_connection.isValid()) {
		m_finished = true;
		return;
	}
	bool ok = m_connection.rollback();
	qfInfo() << "ROLLBACK";
	if(!ok) {
		qfError() << "ROLLBACK transaction error -" << m_connection.lastError().text();
	}
	m_finished = true;
}
