#include "desktoputils.h"

#include <QApplication>
#include <QScreen>

namespace qf {
namespace qmlwidgets {
namespace internal {

QRect DesktopUtils::moveRectToVisibleDesktopScreen(const QRect &rect)
{
	QScreen *scr = QApplication::screenAt(rect.topLeft());
	if(!scr) {
		scr = QApplication::primaryScreen();
	}
	if(!scr) {
		return {};
	}
	QRect screen_rect = scr->geometry();
	if(screen_rect.contains(rect.topLeft())) {
		return rect;
	}
	else {
		auto ret = rect;
		ret.moveCenter(screen_rect.center());
		return ret;
	}
}

} // namespace internal
} // namespace qmlwidgets
} // namespace qf

