#include "qmlwidgetssingleton.h"

QmlWidgetsSingleton::QmlWidgetsSingleton(QObject *parent)
	: QObject(parent)
{
}

QObject *QmlWidgetsSingleton::singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(engine)
	Q_UNUSED(scriptEngine)

	QmlWidgetsSingleton *s = new QmlWidgetsSingleton();
	return s;
}

