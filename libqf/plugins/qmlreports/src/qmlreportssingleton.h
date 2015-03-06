#ifndef QMLREPORTSSINGLETON_H
#define QMLREPORTSSINGLETON_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class QmlReportsSingleton : public QObject
{
	Q_OBJECT
public:
	QmlReportsSingleton(QObject* parent = 0);
public:
	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
};

#endif // QMLREPORTSSINGLETON_H
