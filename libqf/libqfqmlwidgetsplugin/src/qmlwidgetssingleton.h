#ifndef QMLWIDGETSSINGLETON_H
#define QMLWIDGETSSINGLETON_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class QmlWidgetsSingleton : public QObject
{
	Q_OBJECT
public:
	QmlWidgetsSingleton(QObject* parent = 0);
public:
	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
};

#endif // QMLWIDGETSSINGLETON_H
