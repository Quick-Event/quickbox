#include "qmlsqlsingleton.h"
#include "sqlconnection.h"

#include <qf/core/log.h>

#include <QQmlEngine>
#include <QVariant>
#include <QDate>

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

QVariant QmlSqlSingleton::retypeStringValue(const QString &str_val, const QString &type_name)
{
	QVariant ret;
	QByteArray ba = type_name.toLatin1();
	QVariant::Type type = QVariant::nameToType(ba.constData());
	qfInfo() << type << "Converting string value:" << str_val << "to type:" << type_name << "Qt type:" << QVariant::typeToName(type);
	bool ok = true;
	switch (type) {
	case QVariant::Int:
		ret = str_val.toInt(&ok);
		break;
	case QVariant::Double:
		ret = str_val.toDouble(&ok);
		break;
	case QVariant::Date:
	{
		QDate d = QDate::fromString(str_val, Qt::ISODate);
		if(!d.isValid())
			ok = false;
		ret = d;
		break;
	}
	default:
		ret = str_val;
		break;
	}
	if(!ok)
		qfWarning() << "Error converting string value:" << str_val << "to type:" << type_name << "Qt type:" << QVariant::typeToName(type);
	return ret;
}

QString QmlSqlSingleton::typeNameForValue(const QVariant &val)
{
	QString ret = val.typeName();
	return ret;
}
/*
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
*/
