#include "desktoputils.h"

#include <QApplication>
#include <QScreen>

namespace qf {
namespace qmlwidgets {
namespace internal {

QRect DesktopUtils::moveRectToVisibleDesktopScreen(const QRect &rect)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
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
#else
	auto ret = rect;
	QDesktopWidget *dw = QApplication::desktop();
	QRect screen_rect = dw->screenGeometry(ret.topLeft());
	if(screen_rect.isValid() && !screen_rect.contains(ret.topLeft()))
		ret.moveTopLeft(screen_rect.topLeft());
	return ret;
#endif
}


} // namespace internal
} // namespace qmlwidgets
} // namespace qf

