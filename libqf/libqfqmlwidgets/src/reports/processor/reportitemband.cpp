#include "reportitemband.h"

#include "reportprocessor.h"
#include "reportitemdetail.h"
#include "banddatamodel.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/string.h>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

ReportItemBand::ReportItemBand(ReportItem *parent)
	: Super(parent)
{
	qfLogFuncFrame();
}

ReportItemBand::~ReportItemBand()
{
	qfLogFuncFrame();
	QF_SAFE_DELETE(m_model);
}

void ReportItemBand::setData(QVariant d)
{
	if (m_data == d)
		return;
	m_data = d;
	QF_SAFE_DELETE(m_model);
	emit dataChanged(d);

}

BandDataModel *ReportItemBand::model()
{
	if(!m_model) {
		QVariant dta = data();
		if(!dta.isValid() || dta.userType() == QVariant::String) {
			ReportItemBand *pr = parentBand();
			if(pr) {
				BandDataModel *dm = pr->model();
				ReportItemDetail *det = pr->detail();
				if(dm && det) {
					dta = dm->table(det->currentIndex(), dta.toString());
				}
			}
			else {
				/// take data from report processor
				ReportProcessor *proc = processor();
				if(proc) {
					dta = QVariant::fromValue(proc->data());
				}
			}
		}
		m_model = BandDataModel::createFromData(dta, this);
	}
	return m_model;
}

ReportItem::PrintResult ReportItemBand::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfLogFuncFrame() << this;
	//qfInfo() << dataTable().toString();
	/*--
	if(dataTable().isNull() && !processor()->isDesignMode()) { /// pokud neni table (treba bez radku), band se vubec netiskne
		PrintResult res;
		res.value = PrintOk;
		return res;
	}
	--*/
	if(isHeaderOnBreak()) {
		/// print everything except of detail again
		ReportItemDetail *it_det = detail();
		if(it_det)
			it_det->resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
		indexToPrint = 0;
	}
	PrintResult res = Super::printMetaPaint(out, bounding_rect);
	qfDebug() << "\tRETURN:" << res.toString();
	return res;
}

ReportItemDetail *ReportItemBand::detail()
{
	ReportItemDetail *ret = findChild<ReportItemDetail*>(QString(), Qt::FindDirectChildrenOnly);
	return ret;
}

#if 0
//==========================================================
//                                    ReportItemBand
//==========================================================
ReportItemBand::ReportItemBand(ReportItem *parent)
	: ReportItemFrame(parent), dataTableLoaded(false)
{
	qfLogFuncFrame();
}

ReportItemBand::~ReportItemBand()
{
	qfLogFuncFrame();
}

void ReportItemBand::resetIndexToPrintRecursively(bool including_para_texts)
{
	ReportItemFrame::resetIndexToPrintRecursively(including_para_texts);
	dataTableLoaded = false;
	ReportItemDetail *det = detail();
	if(det) {
		//qfInfo() << "resetCurrentRowNo() elid:" << det->element.attribute("id");
		det->resetCurrentRowNo();
	}
}

ReportItemDetail* ReportItemBand::detail()
{
	ReportItemDetail *ret = NULL;
	for(int i=0; i<itemCount(); i++) {
		ReportItem *it = itemAt(i);
		ret = it->toDetail();
		if(ret) break;
	}
	return ret;
}

qfu::TreeTable ReportItemBand::dataTable()
{
	//qfLogFuncFrame() << "dataTableLoaded:" << dataTableLoaded;
	if(!dataTableLoaded) {
		f_dataTable = data();
		if(f_dataTable.isNull()) {
			QString data_src = dataSource();
			f_dataTable = findDataTable(data_src);
		}
		//qfu::TreeTable t =v.value<qfu::TreeTable>();
		dataTableLoaded = true;
	}
	return f_dataTable;
}

ReportItem::PrintResult ReportItemBand::printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect)
{
	qfLogFuncFrame() << this;
	//qfInfo() << dataTable().toString();
	/*--
	if(dataTable().isNull() && !processor()->isDesignMode()) { /// pokud neni table (treba bez radku), band se vubec netiskne
		PrintResult res;
		res.value = PrintOk;
		return res;
	}
	--*/
	if(isHeaderOnBreak()) {
		/// print everything except of detail again
		for(int i=0; i<itemCount(); i++) {
			ReportItem *it = itemAt(i);
			if(it->toDetail() == NULL)
				it->resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
		}
		indexToPrint = 0;
	}
	PrintResult res = ReportItemFrame::printMetaPaint(out, bounding_rect);
	qfDebug() << "\tRETURN:" << res.toString();
	return res;
}

#endif
