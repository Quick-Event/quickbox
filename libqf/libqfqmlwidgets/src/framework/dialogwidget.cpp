#include "dialogwidget.h"

#include "../action.h"
#include "../dialogs/dialog.h"

#include <qf/core/exception.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>

using namespace qf::qmlwidgets::framework;

DialogWidget::DialogWidget(QWidget *parent) :
	Super(parent), IPersistentSettings(this)
{
}

DialogWidget::~DialogWidget()
{
}

bool DialogWidget::acceptDialogDone(int result)
{
	qfLogFuncFrame();
	Q_UNUSED(result);
	return true;
}
/*
QVariant DialogWidget::acceptDialogDone_qml(const QVariant &result)
{
	return acceptDialogDone(result.toBool());
}
*/
void DialogWidget::settleDownInDialog_qml(const QVariant &dlg)
{
	QObject *o = dlg.value<QObject*>();
	qf::qmlwidgets::dialogs::Dialog *pdlg = qobject_cast<qf::qmlwidgets::dialogs::Dialog *>(o);
	QF_ASSERT(pdlg != nullptr, "Invalid dialog", return);
	settleDownInDialog(pdlg);
}

void DialogWidget::settleDownInDialog(qf::qmlwidgets::dialogs::Dialog *dlg)
{
	Q_UNUSED(dlg);
}

qf::qmlwidgets::Action* DialogWidget::action(const QString &name, bool throw_exc)
{
	qfLogFuncFrame() << "name:" << name;
	qf::qmlwidgets::Action *ret = actions().value(name);
	if(!ret && throw_exc)
		QF_EXCEPTION(tr("Cannot find action name '%1'").arg(name));
	return ret;
}

DialogWidget::ActionMap DialogWidget::createActions()
{
	return ActionMap();
}

DialogWidget::ActionMap DialogWidget::actions()
{
	qfLogFuncFrame();
	if(m_actions.isEmpty()) {
		m_actions = createActions();
	}
	return m_actions;
}

