#include "inputdialogsingleton.h"

#include <qf/qmlwidgets/dialogs/getiteminputdialog.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>

#include <qf/core/log.h>

#include <QQmlEngine>
#include <QInputDialog>
#include <QApplication>

InputDialogSingleton::InputDialogSingleton(QObject *parent) :
	QObject(parent)
{
}

QObject *InputDialogSingleton::singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(scriptEngine)
	InputDialogSingleton *s = new InputDialogSingleton(engine);
	return s;
}

QVariant InputDialogSingleton::getInt(QWidget *parent, const QString &title, const QString &label, int value, int min, int max, int step)
{
	bool ok;
	int n = QInputDialog::getInt(parent, title, label, value, min, max, step, &ok);
	QVariant ret;
	if(ok)
		ret = n;
	return ret;
}

QString InputDialogSingleton::getText(QWidget *parent, const QString &title, const QString &label, const QString &text)
{
	if(parent == nullptr) {
		parent = QApplication::activeWindow();
	}
	bool ok;
	QString ret = QInputDialog::getText(parent, title, label, QLineEdit::Normal, text, &ok);
	if(!ok) {
		ret = QString();
	}
	return ret;
}

QString InputDialogSingleton::getItem(QWidget *parent, const QString &title, const QString &label, const QStringList &items, int current, bool editable)
{
	qfLogFuncFrame();
	if(parent == nullptr) {
		parent = QApplication::activeWindow();
	}
	bool ok;
	QString ret = QInputDialog::getItem(parent, title, label, items, current, editable, &ok);
	if(!ok) {
		ret = QString();
	}
	return ret;
}

int InputDialogSingleton::getItemIndex(QWidget *parent, const QString &title, const QString &label, const QStringList &items, int current)
{
	qfLogFuncFrame();
	int ret = -1;
	if(parent == nullptr) {
		parent = QApplication::activeWindow();
	}
	qf::qmlwidgets::dialogs::GetItemInputDialog dlg(parent);
	dlg.setWindowTitle(title);
	dlg.setLabelText(label);
	dlg.setItems(items);
	dlg.setCurrentItemIndex(current);
	bool ok = dlg.exec();
	if(ok) {
		ret = dlg.currentItemIndex();
	}
	return ret;
}

QString InputDialogSingleton::getOpenFileName(QWidget *parent, const QString &title, const QString &dir, const QString &filter)
{
	return qf::qmlwidgets::dialogs::FileDialog::getOpenFileName(parent, title, dir, filter);
}

QStringList InputDialogSingleton::getOpenFileNames(QWidget *parent, const QString &title, const QString &dir, const QString &filter)
{
	return qf::qmlwidgets::dialogs::FileDialog::getOpenFileNames(parent, title, dir, filter);
}

QString InputDialogSingleton::getSaveFileName(QWidget *parent, const QString &title, const QString &dir, const QString &filter)
{
	return qf::qmlwidgets::dialogs::FileDialog::getSaveFileName(parent, title, dir, filter);
}

QString InputDialogSingleton::getExistingDirectory(QWidget *parent, const QString &title, const QString &dir)
{
	return qf::qmlwidgets::dialogs::FileDialog::getExistingDirectory(parent, title, dir);
}
