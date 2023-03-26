#include "messagebox.h"
#include "../framework/ipersistentsettings.h"

#include <qf/core/exception.h>
#include <qf/core/assert.h>

#include <QApplication>
#include <QCheckBox>
#include <QSettings>

namespace qf {
namespace qmlwidgets {
namespace dialogs {

MessageBox::MessageBox(QWidget *parent) :
	Super(parent)
{
}

int MessageBox::exec()
{
	if(loadShowAgainDisabled())
		return QMessageBox::Ok;
	QCheckBox *show_again_cbx = nullptr;
	QString id = showAgainPersistentSettingsId();
	if(!id.isEmpty()) {
		show_again_cbx = new QCheckBox(tr("Show this message again"));
		show_again_cbx->setChecked(true);
		setCheckBox(show_again_cbx);
	}
	int ret = Super::exec();
	if(show_again_cbx && !show_again_cbx->isChecked())
		saveShowAgainDisabled(true);
	return ret;
}

void MessageBox::showException(QWidget *parent, const QString &what, const QString &where, const QString &stack_trace)
{
	Q_UNUSED(where);
	Q_UNUSED(stack_trace);
	QString msg = what;
	if(!parent)
		parent = QApplication::activeWindow();
	QMessageBox::critical(parent, tr("Exception"), msg);
}

void MessageBox::showException(QWidget *parent, const qf::core::Exception &exc)
{
	showException(parent, exc.message(), exc.where(), exc.stackTrace());
}

void MessageBox::showError(QWidget *parent, const QString &message)
{
	QMessageBox::critical(parent, tr("Error"), message);
}

void MessageBox::showWarning(QWidget *parent, const QString &message)
{
	QMessageBox::warning(parent, tr("Warning"), message);
}

void MessageBox::showInfo(QWidget *parent, const QString &message)
{
	QMessageBox::information(parent, tr("Information"), message);
}

bool MessageBox::askYesNo(QWidget *parent, const QString &msg, bool default_ret)
{
	auto i_def = (default_ret) ? StandardButton::No : StandardButton::Yes;
	int i = QMessageBox::question(parent, tr("Question"), msg, StandardButtons(Yes | No), i_def);
	return i == StandardButton::Yes;
}

bool MessageBox::loadShowAgainDisabled()
{
	QString id = showAgainPersistentSettingsId();
	if(id.isEmpty()) {
		return true;
	}
	QSettings settings;
	bool ret = settings.value(id).toBool();
	return ret;
}

void MessageBox::saveShowAgainDisabled(bool b)
{
	QString id = showAgainPersistentSettingsId();
	QF_ASSERT(!id.isEmpty(), "Cannot set show again enabled with 'doNotShowAgainPersistentKey' property not set!", return);
	QSettings settings;
	settings.setValue(id, b);
}

QString MessageBox::showAgainPersistentSettingsId()
{
	QString key = doNotShowAgainPersistentKey();
	if(key.isEmpty()) {
		return QString();
	}
	return framework::IPersistentSettings::defaultPersistentSettingsPathPrefix() + QStringLiteral("/doNotShowMessageAgain/") + key;
}

}}}
