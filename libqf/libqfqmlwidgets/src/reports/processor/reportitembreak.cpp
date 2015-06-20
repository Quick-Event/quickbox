#include "reportitembreak.h"

namespace qf {
namespace qmlwidgets {
namespace reports {

ReportItemBreak::ReportItemBreak(ReportItem *parent)
	: Super(parent)
{
	/// attribut type (page | column) zatim nedela nic
	//QF_ASSERT_EX(proc, "processor is NULL", return);
	designedRect.verticalUnit = Rect::UnitInvalid;
	//qfInfo() << element.attribute("id");
	m_breaking = false;
}

ReportItem::PrintResult ReportItemBreak::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect )
{
	qfLogFuncFrame() << "is breaking:" << m_breaking;
	Q_UNUSED(bounding_rect);
	Q_UNUSED(out);
	PrintResult res = PR_PrintedOk;
	if(!isVisible()) {
		return res;
	}
	if(!m_breaking) {
		res = PR_PrintAgainOnNextPage;
		//res.flags = FlagPrintBreak;
	}
	m_breaking = !m_breaking;
	return res;
}

} // namespace reports
} // namespace qmlwidgets
} // namespace qf

