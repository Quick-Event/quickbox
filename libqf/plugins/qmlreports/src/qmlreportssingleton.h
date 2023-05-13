#ifndef QMLREPORTSSINGLETON_H
#define QMLREPORTSSINGLETON_H

#include <QObject>
#include <QQmlComponent>

class QQmlEngine;
class QJSEngine;

class QmlReportsSingleton : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	QmlReportsSingleton(QObject* parent = 0);
public:
	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
};

#endif // QMLREPORTSSINGLETON_H
