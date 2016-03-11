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
	PrintResult ret = PrintResult::createPrintFinished();
	if(!m_breaking) {
		ret = PrintResult::createPrintAgain();
		if(breakType() == BreakType::Page)
			ret.setPageBreak(true);
		else if(breakType() == BreakType::Column)
			ret.setColumnBreak(true);
	}
	m_breaking = !m_breaking;
	return ret;
}

} // namespace reports
} // namespace qmlwidgets
} // namespace qf

