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
#include "reportitemband.h"
#include "reportitemdetail.h"
#include "banddatamodel.h"
#include "reportitempara.h"
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
	PrintResult res = PR_PrintedOk;
	if(out.isNull())
		return res;

	qfDebug() << "\tparent html element:" << out.tagName();
	if(itemsToPrintCount() > 0) {
		if(itemsToPrintCount() == 1) {
			/// jedno dite vyres tak, ze se vubec nevytiskne rodicovsky frame
			ReportItem *it = itemToPrintAt(0);
			res = it->printHtml(out);
		}
		else {
			QDomElement el_div = out.ownerDocument().createElement("div");;
			if(layout() == LayoutHorizontal) {
				el_div.setAttribute(ReportProcessor::htmlAttributeName_layout(), QStringLiteral("horizontal"));
			}
			for(int i=0; i<itemsToPrintCount(); i++) {
				ReportItem *it = itemToPrintAt(i);
				PrintResult ch_res;
				do {
					ch_res = it->printHtml(el_div);
				} while(ch_res == PR_PrintAgainDetail);
				res = ch_res;
			}
			out.appendChild(el_div);
		}
	}
	return res;
}

//===================================================================
//                           ReportItemBand
//===================================================================

ReportItem::PrintResult ReportItemBand::printHtml(ReportItem::HTMLElement &out)
{
	qfLogFuncFrame() << this;
	PrintResult res = Super::printHtml(out);
	if(res == PR_PrintedOk) {
		QDomElement el_band = out.lastChild().toElement();
		el_band.setAttribute(ReportProcessor::htmlAttributeName_item(), QStringLiteral("band"));
		for(QDomElement el = el_band.firstChildElement(); !el.isNull(); el = el.nextSiblingElement()) {
			if(el.tagName() == QStringLiteral("div")) {
				if(el.attribute(ReportProcessor::htmlAttributeName_item()) != QStringLiteral("detail")) {
					/// non detail rows in band should be exported to HTML as rows
					el.setAttribute(ReportProcessor::htmlAttributeName_item(), QStringLiteral("header"));
				}
			}
		}
	}
	return res;
}

//===================================================================
//                           ReportItemDetail
//===================================================================
ReportItem::PrintResult ReportItemDetail::printHtml(HTMLElement & out)
{
	qfLogFuncFrame() << "current index:" << currentIndex();
	ReportItemBand *band = qobject_cast<ReportItemBand*>(parent());
	BandDataModel *model = nullptr;
	if(band) {
		model = band->model();
		if(model) {
			if(currentIndex() < 0) {
				setCurrentIndex(0);
			}
		}
	}
	PrintResult res;
	res = Super::printHtml(out);
	if(res == PR_PrintedOk) {
		{
			QDomElement el = out.lastChild().toElement();
			el.setAttribute(ReportProcessor::htmlAttributeName_item(), QStringLiteral("detail"));
		}
		if(model) {
			/// take next data row
			int ix = currentIndex() + 1;
			if(ix < model->rowCount()) {
				setCurrentIndex(ix);
				resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
				res = PR_PrintAgainDetail;
			}
			else {
				resetCurrentIndex();
			}
		}
	}
	return res;
}

//===================================================================
//                           ReportItemPara
//===================================================================
ReportItem::PrintResult ReportItemPara::printHtml(HTMLElement & out)
{
	qfLogFuncFrame();// << element.tagName() << "id:" << element.attribute("id");
	PrintResult res = PR_PrintedOk;
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
