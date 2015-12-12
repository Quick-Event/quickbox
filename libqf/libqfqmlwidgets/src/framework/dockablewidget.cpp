#include "dockablewidget.h"

#include <qf/core/log.h>

namespace qf {
namespace qmlwidgets {
namespace framework {

DockableWidget::DockableWidget(QWidget *parent)
	: Super(parent)
{

}

void DockableWidget::onDockWidgetVisibleChanged(bool visible)
{
	qfLogFuncFrame() << this << visible;
}

} // namespace framework
} // namespace qmlwidgets
} // namespace qf

