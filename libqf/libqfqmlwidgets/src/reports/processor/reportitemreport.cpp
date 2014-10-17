#include "reportitemreport.h"

namespace qfc = qf::core;
namespace qfu = qf::core::utils;
using namespace qf::qmlwidgets::reports;

//==========================================================
//                   ReportItemReport
//==========================================================
ReportItemReport::ReportItemReport(ReportItem *parent)
	: Super(parent), m_reportProcessor(nullptr)
{
	qfLogFuncFrame();
	//QF_ASSERT(parent, "processor is NULL");
	//Rect r = designedRect;
	//QDomElement el = element.cloneNode(false).toElement();
	//qfDebug() << "\toriginal:" << element.tagName() << "is null:" << element.isNull() << "has children:" << element.hasChildNodes() << "parent node is null:" << element.parentNode().isNull();
	//qfDebug() << "\tclone:" << el.tagName() << "is null:" << el.isNull() << "has children:" << el.hasChildNodes() << "parent node is null:" << el.parentNode().isNull();
	/*--
	if(element.attribute("orientation") == "landscape") {
		Size sz = designedRect.size();
		sz.transpose();
		designedRect.setSize(sz);
	}
	designedRect.flags = (Rect::LeftFixed | Rect::TopFixed | Rect::RightFixed | Rect::BottomFixed);
	--*/
	//--f_dataTable = parent->data();
	//--dataTableLoaded = true;
}

ReportItemReport::~ReportItemReport()
{
	qfLogFuncFrame();
}

ReportItem::PrintResult ReportItemReport::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect )
{
	qfLogFuncFrame() << "\x1B[1;31;40m***ROOT***ROOT***ROOT***ROOT***\x1B[0;37;40m" << this;
	Q_UNUSED(bounding_rect);
	PrintResult res = PrintOk;
	//updateChildren();
	//ReportItemMetaPaintPage *pg = new ReportItemMetaPaintPage(out, element, processor()->context());
	//pg->renderedRect = designedRect;
	//indexToPrint = 0; /// vzdy vytiskni header a footer. (footer je absolutni header, umisteny pred detailem)
	res = Super::printMetaPaint(out, designedRect);
	//res = printMetaPaintChildren(pg, pg->renderedRect);
	qfDebug() << "\t\x1B[1;31;40m<<< ***ROOT***ROOT***ROOT***ROOT***\x1B[0;37;40m";
	//res = checkPrintResult(res);
	return res;
}
/*--
qfu::TreeTable ReportItemReport::dataTable()
{
	return f_dataTable;
}
--*/

