#include "desktoputils.h"

#include <QApplication>
#include <QScreen>

namespace qf {
namespace qmlwidgets {
namespace internal {

QRect DesktopUtils::moveRectToVisibleDesktopScreen(const QRect &r)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
	if(QApplication::screenAt(r.topLeft())) {
		return r;
	}
	else {
		if(auto *first_screen = QApplication::screens().value(0)) {
			auto ret = r;
			ret.moveTopLeft(first_screen->geometry().topLeft());
			return ret;
		}
		return r;
	}
#else
	return ret;
#endif
}

} // namespace internal
} // namespace qmlwidgets
} // namespace qf

