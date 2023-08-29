#ifndef QF_QMLWIDGETS_INTERNAL_DESKTOPUTILS_H
#define QF_QMLWIDGETS_INTERNAL_DESKTOPUTILS_H

class QRect;

namespace qf {
namespace qmlwidgets {
namespace internal {

class DesktopUtils
{
public:
	static QRect moveRectToVisibleDesktopScreen(const QRect &rect);
};

} // namespace internal
} // namespace qmlwidgets
} // namespace qf

#endif // QF_QMLWIDGETS_INTERNAL_DESKTOPUTILS_H
