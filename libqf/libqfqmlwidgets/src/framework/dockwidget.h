#ifndef QF_QMLWIDGETS_FRAMEWORK_DOCKWIDGET_H
#define QF_QMLWIDGETS_FRAMEWORK_DOCKWIDGET_H

#include "../qmlwidgetsglobal.h"
#include "../framework/ipersistentsettings.h"

#include <QDockWidget>

namespace qf {
namespace qmlwidgets {
class Frame;
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT DockWidget : public QDockWidget, public framework::IPersistentSettings
{
	Q_OBJECT
	Q_PROPERTY(QWidget* widget READ widget WRITE setQmlWidget)
	Q_CLASSINFO("DefaultProperty", "widget")
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
private:
	typedef QDockWidget Super;
public:
	explicit DockWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~DockWidget() Q_DECL_OVERRIDE;

	Q_SIGNAL void visibleChanged(bool visible);
protected:
	void showEvent(QShowEvent *ev) Q_DECL_OVERRIDE;
private:
	void setQmlWidget(QWidget *w);
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_DOCKWIDGET_H
