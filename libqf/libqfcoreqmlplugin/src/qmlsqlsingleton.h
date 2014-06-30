#ifndef QF_CORE_QML_QMLSQLSINGLETON_H
#define QF_CORE_QML_QMLSQLSINGLETON_H

#include <QObject>
#include <QSqlDatabase>

class QQmlEngine;
class QJSEngine;

namespace qf {
namespace core {
namespace qml {

class SqlDatabase;

class QmlSqlSingleton : public QObject
{
	Q_OBJECT
public:
	QmlSqlSingleton(QObject *parent = 0);

	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
public:
	Q_INVOKABLE void addDatabase(const QString &type, const QString &connection_name = QLatin1String(QSqlDatabase::defaultConnection));
	Q_INVOKABLE qf::core::qml::SqlDatabase* createDatabase(const QString &connection_name = QLatin1String(QSqlDatabase::defaultConnection));
};

}}}

#endif
