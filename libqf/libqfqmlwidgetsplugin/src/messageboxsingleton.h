#ifndef MESSAGEBOXSINGLETON_H
#define MESSAGEBOXSINGLETON_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class MessageBoxSingleton : public QObject
{
	Q_OBJECT
public:
	explicit MessageBoxSingleton(QObject *parent = 0);
public:
	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
public:
	Q_INVOKABLE void aboutQt(QWidget *parent = nullptr, const QString &title = QString());
	Q_INVOKABLE int critical(QWidget *parent, const QString &text);
};

#endif // MESSAGEBOXSINGLETON_H
