#ifndef QF_QMLWIDGETS_FRAMEWORK_DIALOGWIDGET_H
#define QF_QMLWIDGETS_FRAMEWORK_DIALOGWIDGET_H

#include "../qmlwidgetsglobal.h"
#include "ipersistentsettings.h"
#include "../frame.h"

#include <qf/core/utils.h>

#include <QWidget>

namespace qf {
namespace qmlwidgets {

class Action;

namespace dialogs {
class Dialog;
}

namespace framework {

class QFQMLWIDGETS_DECL_EXPORT DialogWidget : public Frame, public IPersistentSettings
{
	Q_OBJECT

	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	/// url or path to the icon
	Q_PROPERTY(QString iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
private:
	typedef Frame Super;
public:
	typedef QMap<QString, qf::qmlwidgets::Action*> ActionMap;
public:
	explicit DialogWidget(QWidget *parent = 0);
	~DialogWidget() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL(QString, t, T, itle)
	QF_PROPERTY_IMPL(QString, i, I, conSource)

	/// called when dialog wants to get close
	/// if returned value is false, close action is cancelled
	Q_SLOT virtual bool dialogDoneRequest(int result);
	Q_SLOT bool dialogDoneRequestNative(int result) {return dialogDoneRequest(result);}
public:
	/// define this slot to allow QML code call C++ settleDownInDialog() implementation,
	/// when settleDownInDialog() is implemented in QML and hides C++ implementations
	Q_SLOT void settleDownInDialogNative(qf::qmlwidgets::dialogs::Dialog *dlg);
	Q_SLOT virtual void settleDownInDialog(qf::qmlwidgets::dialogs::Dialog *dlg);
	virtual ActionMap actions();
	virtual qf::qmlwidgets::Action* action(const QString &name, bool throw_exc = true);
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_DIALOGWIDGET_H
