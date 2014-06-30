#include "sqldatabase.h"

#include <qf/core/log.h>

#include <QSqlError>

using namespace qf::core::qml;

SqlDatabase::SqlDatabase(const QString &connection_name, QObject *parent) :
	QObject(parent)
{
	m_sqlDatabase = QSqlDatabase::database(connection_name, false);
}

void SqlDatabase::setConnectionName(const QString &n)
{
	if(n != connectionName()) {
		m_sqlDatabase = QSqlDatabase::database(n, false);
		emit connectionNameChanged();
	}
}

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
		qfInfo() << "OK";
	}
	return ret;
}
