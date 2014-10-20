#include "sqlconnection.h"
#include "sqlquery.h"
#include "sqlquerybuilder.h"

#include <qf/core/log.h>

#include <QSqlError>
#include <QSqlDriver>

namespace qfs = qf::core::sql;

using namespace qf::core::qml;

SqlConnection::SqlConnection(QObject *parent) :
	QObject(parent), m_sqlQuery(nullptr)
{
	qfLogFuncFrame() << this;
	m_sqlConnection = qfs::Connection(QSqlDatabase::database(QSqlDatabase::defaultConnection, false));
	qfDebug() << "created db connection name:" << connectionName();
}

SqlConnection::~SqlConnection()
{
	qfLogFuncFrame() << this << connectionName();
	if(parent()) {
		//qfWarning() << "SqlDatabase has parent:" << parent();
	}
}

void SqlConnection::setConnectionName(const QString &n)
{
	qfLogFuncFrame() << this << connectionName() << "->" << n;
	if(n != connectionName()) {
		m_sqlConnection = qfs::Connection(QSqlDatabase::database(n, false));
		emit connectionNameChanged();
	}
}

QString SqlConnection::defaultConnectionName() const
{
	static QString s = QLatin1String(QSqlDatabase::defaultConnection);
	return s;
}

void SqlConnection::setHostName(const QString &n)
{
	if(n != hostName()) {
		m_sqlConnection.setHostName(n);
		emit hostNameChanged();
	}
}

void SqlConnection::setUserName(const QString &n)
{
	if(n != userName()) {
		m_sqlConnection.setUserName(n);
		emit userNameChanged();
	}
}

void SqlConnection::setPassword(QString n)
{
	if(n != password()) {
		m_sqlConnection.setPassword(n);
		emit databaseNameChanged();
	}
}

void SqlConnection::setDatabaseName(const QString &n)
{
	if(n != databaseName()) {
		m_sqlConnection.setDatabaseName(n);
		emit databaseNameChanged();
	}
}

void SqlConnection::setPort(int n)
{
	if(n != port()) {
		m_sqlConnection.setPort(n);
		emit portChanged();
	}
}

QString SqlConnection::driverName()
{
    return m_sqlConnection.driverName();
}

QString SqlConnection::errorString()
{
    return m_sqlConnection.lastError().text();
}
/*
void SqlDatabase::reloadConnection()
{
	qfLogFuncFrame() << connectionName() << driverName();
	m_sqlDatabase = QSqlDatabase::database(connectionName(), false);
	qfDebug() << connectionName() << driverName();
}
*/
bool SqlConnection::open()
{
	qfInfo() << "Opening database:"
			 << "host:" << m_sqlConnection.hostName()
			 << "user:" << m_sqlConnection.userName()
			 << "port:" << m_sqlConnection.port()
			 << "database:" << m_sqlConnection.databaseName() ;
	bool ret = m_sqlConnection.open();
	if(!ret) {
		qfWarning() << "Open database error:" << m_sqlConnection.lastError().databaseText() << m_sqlConnection.lastError().driverText();
	}
	else {
		emit isOpenChanged();
		qfInfo() << "OK";
	}
	return ret;
}

void SqlConnection::close()
{
	if(isOpen()) {
		m_sqlConnection.close();
		emit isOpenChanged();
	}
}

bool SqlConnection::transaction()
{
	bool ret = true;
	if(m_sqlConnection.driver()->hasFeature(QSqlDriver::Transactions)) {
		ret = m_sqlConnection.transaction();
		if(!ret) {
			qfWarning() << "Cannot open transaction";
		}
	}
	return ret;
}

bool SqlConnection::commit()
{
	bool ret = true;
	if(m_sqlConnection.driver()->hasFeature(QSqlDriver::Transactions)) {
		ret = m_sqlConnection.commit();
		if(!ret) {
			qfWarning() << "Cannot commit transaction";
		}
	}
	return ret;
}

bool SqlConnection::rollback()
{
	bool ret = true;
	if(m_sqlConnection.driver()->hasFeature(QSqlDriver::Transactions)) {
		ret = m_sqlConnection.rollback();
		if(!ret) {
			qfWarning() << "Cannot rollback transaction";
		}
	}
	return ret;
}

SqlQuery *SqlConnection::query()
{
	if(m_sqlQuery == nullptr) {
		m_sqlQuery = createQuery();
	}
	return m_sqlQuery;
}

SqlQuery *SqlConnection::createQuery()
{
	SqlQuery *ret = new SqlQuery();
	ret->setDatabase(m_sqlConnection);
	return ret;
}
/*
SqlQuery *SqlDatabase::exec(const QString &query_str)
{
	SqlQuery *ret = createQuery();
	ret->exec(query_str);
	return ret;
}

SqlQuery *SqlDatabase::exec(SqlQueryBuilder *qb)
{
	return exec(qb->toString());
}
*/
