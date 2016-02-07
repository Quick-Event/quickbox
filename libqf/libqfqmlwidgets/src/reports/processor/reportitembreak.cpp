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
	if(!isVisible()) {
		return PrintResult::createPrintFinished();
	}
	if(!m_breaking) {
		PrintResult res = PrintResult::createPrintAgain();
		if(breakType() == BreakType::Page)
			res.setPageBreak(true);
		else if(breakType() == BreakType::Column)
			res.setColumnBreak(true);
		return res;
	}
	m_breaking = !m_breaking;
	return PrintResult::createPrintFinished();
}

} // namespace reports
} // namespace qmlwidgets
} // namespace qf

