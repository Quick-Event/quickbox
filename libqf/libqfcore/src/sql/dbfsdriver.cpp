#include "dbfsdriver.h"

#include <QSqlDatabase>

using namespace qf::core::sql;

DbFsDriver::DbFsDriver(QObject *parent)
	: QObject(parent)
{
	m_connectionName = QLatin1String(QSqlDatabase::defaultConnection);
	m_tableName = QStringLiteral("dbfs");
}

DbFsDriver::~DbFsDriver()
{
}

