#ifndef INPUTDIALOGSINGLETON_H
#define INPUTDIALOGSINGLETON_H

#include <QObject>
#include <QStringList>
#include <QVariant>

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
	Q_INVOKABLE QVariant getInt(QWidget * parent, const QString &title, const QString & label, int value = 0, int min = -2147483647, int max = 2147483647, int step = 1);
	Q_INVOKABLE QString getText(QWidget *parent, const QString &title, const QString &label, const QString &text = QString());
	Q_INVOKABLE QString getItem(QWidget *parent, const QString &title, const QString &label, const QStringList &items, int current = 0, bool editable = true);
	Q_INVOKABLE int getItemIndex(QWidget *parent, const QString &title, const QString &label, const QStringList &items, int current = 0);

	Q_INVOKABLE QString getOpenFileName(QWidget *parent = 0, const QString &title = QString(), const QString &dir = QString(), const QString &filter = QString());
	Q_INVOKABLE QStringList getOpenFileNames(QWidget *parent = 0, const QString &title = QString(), const QString &dir = QString(), const QString &filter = QString());
	Q_INVOKABLE QString getSaveFileName(QWidget * parent = 0, const QString & title = QString(), const QString & dir = QString(), const QString & filter = QString());
	Q_INVOKABLE QString getExistingDirectory(QWidget * parent = 0, const QString & title = QString(),  const QString & dir = QString());
};

#endif // INPUTDIALOGSINGLETON_H
