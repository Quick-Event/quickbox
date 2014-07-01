#include "qmlwidgetssingleton.h"

#include <QQmlEngine>

QmlWidgetsSingleton::QmlWidgetsSingleton(QObject *parent)
	: QObject(parent)
{
}

QObject *QmlWidgetsSingleton::singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(scriptEngine)
	QmlWidgetsSingleton *s = new QmlWidgetsSingleton(engine);
	return s;
}


