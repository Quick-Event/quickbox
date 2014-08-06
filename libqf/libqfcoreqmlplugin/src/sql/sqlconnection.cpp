#include "sqlconnection.h"
#include "sqlquery.h"
#include "sqlquerybuilder.h"

#include <qf/core/log.h>

#include <QSqlError>
#include <QSqlDriver>

using namespace qf::core::qml;

SqlConnection::SqlConnection(QObject *parent) :
	QObject(parent), m_sqlQuery(nullptr)
{
	qfLogFuncFrame() << this;
	m_sqlDatabase = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
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
		m_sqlDatabase = QSqlDatabase::database(n, false);
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
		m_sqlDatabase.setHostName(n);
		emit hostNameChanged();
	}
}

void SqlConnection::setUserName(const QString &n)
{
	if(n != userName()) {
		m_sqlDatabase.setUserName(n);
		emit userNameChanged();
	}
}

void SqlConnection::setPassword(QString n)
{
	if(n != password()) {
		m_sqlDatabase.setPassword(n);
		emit databaseNameChanged();
	}
}

void SqlConnection::setDatabaseName(const QString &n)
{
	if(n != databaseName()) {
		m_sqlDatabase.setDatabaseName(n);
		emit databaseNameChanged();
	}
}

void SqlConnection::setPort(int n)
{
	if(n != port()) {
		m_sqlDatabase.setPort(n);
		emit portChanged();
	}
}

QString SqlConnection::driverName()
{
	return m_sqlDatabase.driverName();
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
			 << "host:" << m_sqlDatabase.hostName()
			 << "user:" << m_sqlDatabase.userName()
			 << "port:" << m_sqlDatabase.port()
			 << "database:" << m_sqlDatabase.databaseName() ;
	bool ret = m_sqlDatabase.open();
	if(!ret) {
		qfWarning() << "Open database error:" << m_sqlDatabase.lastError().databaseText() << m_sqlDatabase.lastError().driverText();
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
		m_sqlDatabase.close();
		emit isOpenChanged();
	}
}

bool SqlConnection::transaction()
{
	bool ret = true;
	if(m_sqlDatabase.driver()->hasFeature(QSqlDriver::Transactions)) {
		ret = m_sqlDatabase.transaction();
		if(!ret) {
			qfWarning() << "Cannot open transaction";
		}
	}
	return ret;
}

bool SqlConnection::commit()
{
	bool ret = true;
	if(m_sqlDatabase.driver()->hasFeature(QSqlDriver::Transactions)) {
		ret = m_sqlDatabase.commit();
		if(!ret) {
			qfWarning() << "Cannot commit transaction";
		}
	}
	return ret;
}

bool SqlConnection::rollback()
{
	bool ret = true;
	if(m_sqlDatabase.driver()->hasFeature(QSqlDriver::Transactions)) {
		ret = m_sqlDatabase.rollback();
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
	ret->setDatabase(m_sqlDatabase);
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
