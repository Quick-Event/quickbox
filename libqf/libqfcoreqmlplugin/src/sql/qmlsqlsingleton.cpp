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
	Q_UNUSED(scriptEngine)

	QmlSqlSingleton *s = new QmlSqlSingleton(engine);
	return s;
}

bool QmlSqlSingleton::addDatabase(const QString &type, const QString &connection_name)
{
	QSqlDatabase db = QSqlDatabase::addDatabase(type, connection_name);
	bool ret = db.isValid();
	if(!ret) {
		qfError() << "Cannot add database type:" << type << "name:" << connection_name;
	}
	return ret;
}

QVariant QmlSqlSingleton::retypeVariant(const QVariant &val, QVariant::Type type)
{
	QVariant ret;
	if(val.type() == type)
		return val;
	switch(val.type()) {
	case QVariant::String: {
		bool ok = true;
		QString str_val = val.toString();
		switch (type) {
		case QVariant::Bool:
			ret = true;
			if(str_val.isEmpty())
				ret = false;
			else if(str_val.length() < 3) {
				QChar c = str_val[0].toLower();
				if(c == 'n' || c == '0' || c == 'f')
					ret = false;
			}
			else if(str_val.compare(QStringLiteral("false"), Qt::CaseInsensitive)) {
				ret = false;
			}
			break;
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
			qfWarning() << "Error converting string value:" << str_val << "to type:" << type;
		break;
	}
	default:
		qfWarning() << "Don't know how to convert:" << val.toString() << "of type:" << val.typeName() << "to type:" << type;
		break;
	}
	return ret;
}

QVariant QmlSqlSingleton::retypeStringValue(const QString &str_val, const QString &type_name)
{
	QByteArray ba = type_name.toLatin1();
	QVariant::Type type = QVariant::nameToType(ba.constData());
	QVariant ret = retypeVariant(str_val, type);
	return ret;
}

QString QmlSqlSingleton::typeNameForValue(const QVariant &val)
{
	QString ret = val.typeName();
	return ret;
}
