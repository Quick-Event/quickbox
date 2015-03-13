#include "desktoputils.h"

#include <QApplication>
#include <QDesktopWidget>

namespace qf {
namespace qmlwidgets {
namespace internal {

QRect DesktopUtils::moveRectToVisibleDesktopScreen(const QRect &r)
{
	QRect ret = r;
	QDesktopWidget *dw = QApplication::desktop();
	QRect screen_rect = dw->screenGeometry(ret.topLeft());
	if(!screen_rect.contains(ret.topLeft()))
		ret.moveTopLeft(screen_rect.topLeft());
	return ret;
}

} // namespace internal
} // namespace qmlwidgets
} // namespace qf

