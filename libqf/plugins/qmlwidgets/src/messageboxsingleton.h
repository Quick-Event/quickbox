#ifndef MESSAGEBOXSINGLETON_H
#define MESSAGEBOXSINGLETON_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class MessageBoxSingleton : public QObject
{
	Q_OBJECT
public:
	explicit MessageBoxSingleton(QObject *parent = nullptr);
public:
	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
public:
	Q_INVOKABLE void aboutQt(QWidget *parent = nullptr, const QString &title = QString());
	Q_INVOKABLE int information(QWidget *parent, const QString &text);
	Q_INVOKABLE int critical(QWidget *parent, const QString &text);
	Q_INVOKABLE int askYesNo(QWidget *parent, const QString &text, bool default_ret = true);//, const QString &title = tr("Question"));
};

#endif // MESSAGEBOXSINGLETON_H
