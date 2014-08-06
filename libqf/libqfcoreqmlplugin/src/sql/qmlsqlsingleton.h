#ifndef QF_CORE_QML_QMLSQLSINGLETON_H
#define QF_CORE_QML_QMLSQLSINGLETON_H

#include "sqlconnection.h"

#include <QObject>
#include <QMap>

class QQmlEngine;
class QJSEngine;

namespace qf {
namespace core {
namespace qml {

class SqlConnection;
class SqlQuery;

class QmlSqlSingleton : public QObject
{
	Q_OBJECT
public:
	QmlSqlSingleton(QObject *parent = 0);
	~QmlSqlSingleton() Q_DECL_OVERRIDE;

	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
public:
	Q_INVOKABLE void addDatabase(const QString &type, const QString &connection_name = QLatin1String(QSqlDatabase::defaultConnection));
	//Q_INVOKABLE qf::core::qml::SqlDatabase* database(const QString &connection_name = QLatin1String(QSqlDatabase::defaultConnection));
private:
	//QMap<QString, SqlDatabase*> m_sqlDatabases;
};

}}}

#endif
