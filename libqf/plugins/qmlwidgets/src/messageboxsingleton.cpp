#include "messageboxsingleton.h"

#include <qf/core/log.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <QQmlEngine>
#include <QMessageBox>
#include <QApplication>

MessageBoxSingleton::MessageBoxSingleton(QObject *parent) :
	QObject(parent)
{
}

QObject *MessageBoxSingleton::singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(scriptEngine)
	MessageBoxSingleton *s = new MessageBoxSingleton(engine);
	return s;
}

void MessageBoxSingleton::aboutQt(QWidget *parent, const QString &title)
{
	if(parent == nullptr) {
		parent = QApplication::activeWindow();
	}
	QMessageBox::aboutQt(parent, title);
}

int MessageBoxSingleton::information(QWidget *parent, const QString &text)
{
	if(parent == nullptr) {
		parent = QApplication::activeWindow();
	}
	//qfDebug() << text;
	return QMessageBox::information(parent, tr("Information"), text);
}

int MessageBoxSingleton::critical(QWidget *parent, const QString &text)
{
	if(parent == nullptr) {
		parent = QApplication::activeWindow();
	}
	qfWarning() << text;
	return QMessageBox::critical(parent, tr("Error"), text);
}

int MessageBoxSingleton::askYesNo(QWidget *parent, const QString &text, bool default_ret)
{
	if(parent == nullptr) {
		parent = QApplication::activeWindow();
	}
	return qf::qmlwidgets::dialogs::MessageBox::askYesNo(parent, text, default_ret);
}
