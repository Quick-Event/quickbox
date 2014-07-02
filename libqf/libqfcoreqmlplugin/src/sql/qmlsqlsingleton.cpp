#include "qmlsqlsingleton.h"
#include "sqldatabase.h"

#include <qf/core/log.h>

#include <QQmlEngine>

using namespace qf::core::qml;

qf::core::qml::QmlSqlSingleton::QmlSqlSingleton(QObject *parent) :
	QObject(parent)
{
	qfLogFuncFrame() << this;
}

QmlSqlSingleton::~QmlSqlSingleton()
{
	qfLogFuncFrame() << this;
}

QObject *qf::core::qml::QmlSqlSingleton::singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	//Q_UNUSED(engine)
	Q_UNUSED(scriptEngine)

	QmlSqlSingleton *s = new QmlSqlSingleton(engine);
	return s;
}

void QmlSqlSingleton::addDatabase(const QString &type, const QString &connection_name)
{
	QSqlDatabase db = QSqlDatabase::addDatabase(type, connection_name);
	if(!db.isValid()) {
		qfError() << "Cannot add database type:" << type << "name:" << connection_name;
	}
}

SqlDatabase *QmlSqlSingleton::database(const QString &connection_name)
{
	qfLogFuncFrame() << connection_name;
	SqlDatabase *ret = m_sqlDatabases.value(connection_name);
	if(!ret) {
		ret = new SqlDatabase(connection_name, this);
		m_sqlDatabases[connection_name] = ret;
	}
	qfDebug() << "\t return:" << ret << ret->connectionName();
	return ret;
}

