#include "qmlreportssingleton.h"

#include <QQmlEngine>

QmlReportsSingleton::QmlReportsSingleton(QObject *parent)
	: QObject(parent)
{
}

QObject *QmlReportsSingleton::singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(scriptEngine)
	QmlReportsSingleton *s = new QmlReportsSingleton(engine);
	return s;
}


