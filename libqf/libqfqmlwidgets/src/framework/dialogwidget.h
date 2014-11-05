#ifndef QF_QMLWIDGETS_FRAMEWORK_DIALOGWIDGET_H
#define QF_QMLWIDGETS_FRAMEWORK_DIALOGWIDGET_H

#include "../qmlwidgetsglobal.h"
#include "ipersistentsettings.h"
#include "../frame.h"

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
private:
	typedef Frame Super;
public:
	explicit DialogWidget(QWidget *parent = 0);
	~DialogWidget() Q_DECL_OVERRIDE;

	typedef QMap<QString, qf::qmlwidgets::Action*> ActionMap;
public:
	virtual void updateDialogUi(qf::qmlwidgets::dialogs::Dialog *dlg);
	virtual ActionMap actions();
	virtual qf::qmlwidgets::Action* action(const QString &name, bool throw_exc = true);
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_DIALOGWIDGET_H
