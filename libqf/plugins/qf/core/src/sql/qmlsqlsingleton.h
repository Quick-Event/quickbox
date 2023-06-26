#ifndef QF_CORE_QML_QMLSQLSINGLETON_H
#define QF_CORE_QML_QMLSQLSINGLETON_H

#include "sqlconnection.h"

#include <QObject>
#include <QVariant>

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
	QmlSqlSingleton(QObject *parent = nullptr);
	~QmlSqlSingleton() Q_DECL_OVERRIDE;

	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
public:
	Q_INVOKABLE bool addDatabase(const QString &type, const QString &connection_name = QLatin1String(QSqlDatabase::defaultConnection));
	QVariant retypeVariant(const QVariant &val, QMetaType::Type type);
	Q_INVOKABLE QVariant retypeStringValue(const QString &str_val, const QString &type_name);
	Q_INVOKABLE QString typeNameForValue(const QVariant &val);
private:
};

}}}

#endif
