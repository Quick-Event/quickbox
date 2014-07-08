#include "inputdialogsingleton.h"

#include <qf/qmlwidgets/dialogs/getiteminputdialog.h>

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
