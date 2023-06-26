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

QVariant QmlSqlSingleton::retypeVariant(const QVariant &val, QMetaType::Type type)
{
	QVariant ret;
#if QT_VERSION_MAJOR >= 6
	int orig_type = val.typeId();
#else
	int orig_type = val.type();
#endif

	if(orig_type == type)
		return val;
	switch(orig_type) {
	case QMetaType::QString: {
		bool ok = true;
		QString str_val = val.toString();
		switch (type) {
		case QMetaType::Bool:
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
		case QMetaType::Int:
			ret = str_val.toInt(&ok);
			break;
		case QMetaType::Double:
			ret = str_val.toDouble(&ok);
			break;
		case QMetaType::QDate:
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
#if QT_VERSION_MAJOR >= 6
	int type = QMetaType::fromName(ba.constData()).id();
#else
	int type = QVariant::nameToType(ba.constData());
#endif
	QVariant ret = retypeVariant(str_val, static_cast<QMetaType::Type>(type));
	return ret;
}

QString QmlSqlSingleton::typeNameForValue(const QVariant &val)
{
	QString ret = val.typeName();
	return ret;
}
