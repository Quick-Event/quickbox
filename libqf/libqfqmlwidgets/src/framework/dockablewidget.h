#ifndef QF_QMLWIDGETS_FRAMEWORK_DOCKABLEWIDGET_H
#define QF_QMLWIDGETS_FRAMEWORK_DOCKABLEWIDGET_H

#include "dialogwidget.h"

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT DockableWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	DockableWidget(QWidget *parent = nullptr);

	virtual void onDockWidgetVisibleChanged(bool visible);
};

} // namespace framework
} // namespace qmlwidgets
} // namespace qf

#endif // QF_QMLWIDGETS_FRAMEWORK_DOCKABLEWIDGET_H
