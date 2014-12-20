#include "sqlconnection.h"
#include "sqlquery.h"
#include "sqlquerybuilder.h"

#include <qf/core/log.h>

#include <QSqlError>
#include <QSqlDriver>
#include <QQmlEngine>

namespace qfs = qf::core::sql;

using namespace qf::core::qml;

SqlConnection::SqlConnection(QObject *parent) :
	QObject(parent)
{
	qfLogFuncFrame() << this;
	//nativeSqlConnection() = qfs::Connection(QSqlDatabase::database(QSqlDatabase::defaultConnection, false));
	//qfDebug() << "created db connection name:" << connectionName();
}

SqlConnection::~SqlConnection()
{
	qfLogFuncFrame() << this << connectionName();
	if(parent()) {
		//qfWarning() << "SqlDatabase has parent:" << parent();
	}
}

qf::core::sql::Connection &SqlConnection::nativeSqlConnection()
{
	if(!m_sqlConnection.isValid()) {
		m_sqlConnection = qfs::Connection(QSqlDatabase::database(connectionName(), false));
	}
	return m_sqlConnection;
}

QString SqlConnection::defaultConnectionName()
{
	static QString s = QLatin1String(QSqlDatabase::defaultConnection);
	return s;
}

void SqlConnection::setHostName(const QString &n)
{
	if(n != hostName()) {
		nativeSqlConnection().setHostName(n);
		emit hostNameChanged();
	}
}

void SqlConnection::setUserName(const QString &n)
{
	if(n != userName()) {
		nativeSqlConnection().setUserName(n);
		emit userNameChanged();
	}
}

void SqlConnection::setPassword(QString n)
{
	if(n != password()) {
		nativeSqlConnection().setPassword(n);
		emit databaseNameChanged();
	}
}

void SqlConnection::setDatabaseName(const QString &n)
{
	if(n != databaseName()) {
		nativeSqlConnection().setDatabaseName(n);
		emit databaseNameChanged();
	}
}

void SqlConnection::setPort(int n)
{
	if(n != port()) {
		nativeSqlConnection().setPort(n);
		emit portChanged();
	}
}

QString SqlConnection::driverName()
{
	return nativeSqlConnection().driverName();
}

QString SqlConnection::errorString()
{
	return nativeSqlConnection().lastError().text();
}

bool SqlConnection::setCurrentSchema(const QString &schema_name)
{
	return nativeSqlConnection().setCurrentSchema(schema_name);
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
			 << "host:" << nativeSqlConnection().hostName()
			 << "user:" << nativeSqlConnection().userName()
			 << "port:" << nativeSqlConnection().port()
			 << "database:" << nativeSqlConnection().databaseName() ;
	bool ret = nativeSqlConnection().open();
	if(!ret) {
		qfWarning() << "Open database error:" << nativeSqlConnection().lastError().databaseText() << nativeSqlConnection().lastError().driverText();
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
		nativeSqlConnection().close();
		emit isOpenChanged();
	}
}

bool SqlConnection::transaction()
{
	//qfInfo() << ">>>>>>>>>>>>>>>>>>>> BEGIN TRANSACTION";
	bool ret = true;
	if(nativeSqlConnection().driver()->hasFeature(QSqlDriver::Transactions)) {
		ret = nativeSqlConnection().transaction();
		if(!ret) {
			qfWarning() << "Cannot open transaction";
		}
	}
	return ret;
}

bool SqlConnection::commit()
{
	//qfInfo() << "<<<<<<<<<<<<<<<<<<<< COMIT TRANSACTION";
	bool ret = true;
	if(nativeSqlConnection().driver()->hasFeature(QSqlDriver::Transactions)) {
		ret = nativeSqlConnection().commit();
		if(!ret) {
			qfWarning() << "Cannot commit transaction";
		}
	}
	return ret;
}

bool SqlConnection::rollback()
{
	//qfWarning() << "<<<<<<<<<<<<<<<<<<<< ROLLBACK TRANSACTION";
	bool ret = true;
	if(nativeSqlConnection().driver()->hasFeature(QSqlDriver::Transactions)) {
		ret = nativeSqlConnection().rollback();
		if(!ret) {
			qfWarning() << "Cannot rollback transaction";
		}
	}
	return ret;
}

SqlQuery *SqlConnection::createQuery()
{
	SqlQuery *ret = new SqlQuery();
	ret->setDatabase(nativeSqlConnection());
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
