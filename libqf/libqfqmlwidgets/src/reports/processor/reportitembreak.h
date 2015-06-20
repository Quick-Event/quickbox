#ifndef QF_QMLWIDGETS_REPORTS_REPORTITEMBREAK_H
#define QF_QMLWIDGETS_REPORTS_REPORTITEMBREAK_H

#include "reportitem.h"

namespace qf {
namespace qmlwidgets {
namespace reports {

class QFQMLWIDGETS_DECL_EXPORT ReportItemBreak : public ReportItem
{
	Q_OBJECT
private:
	typedef ReportItem Super;
public:
	ReportItemBreak(ReportItem *parent = nullptr);
public:
	virtual bool isBreak() {return true;}

	virtual ChildSize childSize(Layout parent_layout) {
		Q_UNUSED(parent_layout);
		return ChildSize(0, Rect::UnitInvalid);
	}
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
private:
	bool m_breaking;
};

} // namespace reports
} // namespace qmlwidgets
} // namespace qf

#endif // QF_QMLWIDGETS_REPORTS_REPORTITEMBREAK_H
