#include "sqldatabase.h"
#include "sqlquery.h"
#include "sqlquerybuilder.h"

#include <qf/core/log.h>

#include <QSqlError>
#include <QSqlDriver>

using namespace qf::core::qml;

SqlDatabase::SqlDatabase(const QString &connection_name, QObject *parent) :
	QObject(parent)
{
	qfLogFuncFrame() << this << connectionName();
	m_sqlDatabase = QSqlDatabase::database(connection_name, false);
}

SqlDatabase::~SqlDatabase()
{
	qfLogFuncFrame() << this << connectionName();
	if(parent()) {
		//qfWarning() << "SqlDatabase has parent:" << parent();
	}
}
/*
void SqlDatabase::setConnectionName(const QString &n)
{
	if(n != connectionName()) {
		m_sqlDatabase = QSqlDatabase::database(n, false);
		emit connectionNameChanged();
	}
}
*/
void SqlDatabase::setHostName(const QString &n)
{
	if(n != hostName()) {
		m_sqlDatabase.setHostName(n);
		emit hostNameChanged();
	}
}

void SqlDatabase::setUserName(const QString &n)
{
	if(n != userName()) {
		m_sqlDatabase.setUserName(n);
		emit userNameChanged();
	}
}

void SqlDatabase::setDatabaseName(const QString &n)
{
	if(n != databaseName()) {
		m_sqlDatabase.setDatabaseName(n);
		emit databaseNameChanged();
	}
}

void SqlDatabase::setPort(int n)
{
	if(n != port()) {
		m_sqlDatabase.setPort(n);
		emit portChanged();
	}
}

QString SqlDatabase::driverName()
{
	return m_sqlDatabase.driverName();
}

bool SqlDatabase::open()
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

void SqlDatabase::close()
{
	if(isOpen()) {
		m_sqlDatabase.close();
		emit isOpenChanged();
	}
}

bool SqlDatabase::transaction()
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

bool SqlDatabase::commit()
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

bool SqlDatabase::rollback()
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

SqlQuery *SqlDatabase::createQuery()
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
