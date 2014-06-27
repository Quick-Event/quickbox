#include "qmlsqlsingleton.h"

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
