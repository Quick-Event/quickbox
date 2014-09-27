#ifndef QF_QMLWIDGETS_FRAMEWORK_CURSOROVERRIDER_H
#define QF_QMLWIDGETS_FRAMEWORK_CURSOROVERRIDER_H

#include "../qmlwidgetsglobal.h"

#include <Qt>

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT CursorOverrider
{
public:
	CursorOverrider(Qt::CursorShape cursor_shape);
	virtual ~CursorOverrider();
};

}}}

#endif // CURSOROVERRIDER_H
