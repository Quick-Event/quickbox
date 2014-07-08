#ifndef INPUTDIALOGSINGLETON_H
#define INPUTDIALOGSINGLETON_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class InputDialogSingleton : public QObject
{
	Q_OBJECT
public:
	InputDialogSingleton(QObject* parent = 0);
public:
	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
public:
	Q_INVOKABLE QString getText(QWidget *parent, const QString &title, const QString &label, const QString &text = QString());
	Q_INVOKABLE QString getItem(QWidget *parent, const QString &title, const QString &label, const QStringList &items, int current = 0, bool editable = true);
	Q_INVOKABLE int getItemIndex(QWidget *parent, const QString &title, const QString &label, const QStringList &items, int current = 0);
};

#endif // INPUTDIALOGSINGLETON_H
