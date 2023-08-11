#include "desktoputils.h"

#include <QApplication>
#include <QScreen>

namespace qf {
namespace qmlwidgets {
namespace internal {

QRect DesktopUtils::moveRectToVisibleDesktopScreen(const QRect &r)
{
	QRect ret = r;
	QScreen *scr = QApplication::screenAt(ret.topLeft());
	QRect screen_rect = scr? scr->geometry(): QRect();
	if(screen_rect.isValid() && !screen_rect.contains(ret.topLeft()))
		ret.moveTopLeft(screen_rect.topLeft());
	return ret;
}

} // namespace internal
} // namespace qmlwidgets
} // namespace qf

