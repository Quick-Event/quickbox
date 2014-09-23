//
// C++ Implementation: Reportitem_html
//
// Description:
//
//
// Author: Fanda Vacek <fanda@JedovaChyse>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "reportitem.h"
#include "reportprocessor.h"
#include "reportpainter.h"

#include <qf/core/log.h>

#include <QDomElement>

namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

namespace {
QDomText setElementText(QDomElement &el, const QString &str)
{
	QDomNode nd = el.firstChild();
	QDomText eltxt = nd.toText();
	if(eltxt.isNull()) {
		eltxt = el.ownerDocument().createTextNode(str);
		el.insertBefore(eltxt, nd);
	}
	else {
		eltxt.setData(str);
	}
	return eltxt;
}
}
//===================================================================
//                           ReportItemFrame
//===================================================================
ReportItem::PrintResult ReportItemFrame::printHtml(HTMLElement & out)
{
	qfLogFuncFrame();
	PrintResult res = PrintOk;
	if(out.isNull())
		return res;

	qfDebug() << "\tparent html element:" << out.tagName();
	//qfDebug() << "\tlayout:" << (isGridLayout()? "grid": (layout() == qf::qmlwidgets::graphics::LayoutHorizontal)? "horizontal": (layout() == qf::qmlwidgets::graphics::LayoutVertical)? "vertical" : "nevim");
	//qfDebug() << "\tmetaPaintLayoutLength:" << metaPaintLayoutLength << "metaPaintOrthogonalLayoutLength:" << metaPaintOrthogonalLayoutLength;
	//--updateChildren();
	if(children().count() > 0) {
		if(children().count() == 1) {
			/// jedno dite vyres tak, ze se vubec nevytiskne rodicovsky frame
			ReportItem *it = childAt(0);
			res = it->printHtml(out);
		}
		else {
			QDomElement el_div = out.ownerDocument().createElement("div");;
			if(layout() == LayoutHorizontal) {
				el_div.setAttribute("layout", "horizontal");
			}
			for(int i=0; i<children().count(); i++) {
				ReportItem *it = childAt(i);
				PrintResult ch_res;
				//int cnt = 0;
				do {
					//if(cnt) qfInfo() << "\t opakovacka:" << cnt;
					ch_res = it->printHtml(el_div);
					//if(cnt) qfInfo() << "\t again2:" << (ch_res .flags & FlagPrintAgain);
					//cnt++;
				} while(ch_res.flags & FlagPrintAgain);
				res = ch_res;
			}
			out.appendChild(el_div);
		}
		/*--
		QDomElement el = out.lastChild().toElement();
		if(!el.isNull()) {
			ReportItemTable *tbl_it = dynamic_cast<ReportItemTable*>(this);
			if(tbl_it) {
				el.setAttribute("__table", "__fakeBandTable");
			}
			else {
				static QStringList sl = QStringList() << "__fakeBandDetail" << "__fakeBandHeaderRow" << "__fakeBandFooterRow";
				foreach(QString s, sl) {
					if(element.attribute(s).toInt() > 0) el.setAttribute("__table", s);
				}
			}
		}
		--*/
	}
	return res;
}

//===================================================================
//                           ReportItemDetail
//===================================================================
ReportItem::PrintResult ReportItemDetail::printHtml(HTMLElement & out)
{
	qfLogFuncFrame();// << "id:" << element.attribute("id");
	//qfDebug().color(QFLog::Yellow) << "\treturn:" << res.toString();
	//qfInfo() << "design mode:" << design_mode;
	bool design_mode = processor->isDesignMode();
	ReportItemBand *b = parentBand();
	qfu::TreeTable data_table;
	if(b) {
		//qfDebug() << "band:" << b << "\ttable is null:" << b->dataTable().isNull();
		data_table = b->dataTable();
		if(!data_table.isNull()) {
			//design_view = false;
			if(f_currentRowNo < 0) {
				/// kdyz neni f_dataRow, vezmi prvni radek dat
				f_currentRowNo = 0;
				//f_dataRow = b->dataTable().firstRow();
				//qfDebug() << "\tfirst row is null:" << f_dataRow.isNull();
				//qfInfo() << "vezmi prvni radek dat element id:" << element.attribute("id") << "f_currentRowNo:" << f_currentRowNo;
			}
		}
	}
	PrintResult res;
	if(!design_mode && (data_table.isNull() || dataRow().isNull())) {
		/// prazdnej detail vubec netiskni
		res.value = PrintOk;
		return res;
	}
	res = Super::printHtml(out);
	if(res.value == PrintOk) {
		if(b) {
			/// vezmi dalsi radek dat
			//qfInfo() << "vezmi dalsi radek dat" << element.attribute("id");
			//f_dataRow = b->dataTable().nextRow(f_dataRow);
			f_currentRowNo++;
			//qfInfo() << "vezmi dalsi radek dat element id:" << element.attribute("id") << "f_currentRowNo:" << f_currentRowNo;
			if(f_currentRowNo < data_table.rowCount()) {
				resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
				res.flags |= FlagPrintAgain;
			}
			//else qfInfo() << "\t IS NULL";
		}
	}
	//res = checkPrintResult(res);
	//qfInfo() << "\t again:" << (res .flags & FlagPrintAgain);
	qfDebug() << "\treturn:" << res.toString();
	return res;
}

//===================================================================
//                           ReportItemPara
//===================================================================
ReportItem::PrintResult ReportItemPara::printHtml(HTMLElement & out)
{
	qfLogFuncFrame();// << element.tagName() << "id:" << element.attribute("id");
	PrintResult res = PrintOk;
	if(out.isNull()) return res;

	QDomElement el_div = out.ownerDocument().createElement("div");
	QDomElement el_p = out.ownerDocument().createElement("p");
	QString text = paraText();
	QRegExp rx = ReportItemMetaPaint::checkReportSubstitutionRegExp;
	if(rx.exactMatch(text)) {
		bool check_on = rx.capturedTexts().value(1) == "1";
		text = (check_on)? "X": QString();
	}
	setElementText(el_p, text);
	out.appendChild(el_div);
	el_div.appendChild(el_p);
	return res;
}
