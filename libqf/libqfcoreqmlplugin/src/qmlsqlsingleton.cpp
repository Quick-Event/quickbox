#include "qmlsqlsingleton.h"
#include "sqldatabase.h"

#include <qf/core/log.h>

#include <QQmlEngine>

using namespace qf::core::qml;

qf::core::qml::QmlSqlSingleton::QmlSqlSingleton(QObject *parent) :
	QObject(parent)
{

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
	QSqlDatabase::addDatabase(type, connection_name);
}

SqlDatabase *QmlSqlSingleton::database(const QString &connection_name)
{
	SqlDatabase *ret = new SqlDatabase(connection_name);
	return ret;
}
