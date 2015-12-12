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
	explicit DockWidget(const QString &window_title, QWidget *parent = nullptr, Qt::WindowFlags flags = 0);
	explicit DockWidget(QWidget *parent = 0, Qt::WindowFlags flags = 0) : DockWidget(QString(), parent, flags) {}
	~DockWidget() Q_DECL_OVERRIDE;

	// visibilityChanged() exists already in QDockWidget
	//Q_SIGNAL void visibleChanged(bool visible);
protected:
	bool event(QEvent *ev) Q_DECL_OVERRIDE;
	//void showEvent(QShowEvent *ev) Q_DECL_OVERRIDE;
private:
	void setQmlWidget(QWidget *w);
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_DOCKWIDGET_H
