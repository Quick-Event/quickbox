#include "reportitemrepeater.h"

#include "reportprocessor.h"
#include "repeatermodel.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/string.h>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

ReportItemRepeater::ReportItemRepeater(ReportItem *parent)
	: Super(parent)
{
	qfLogFuncFrame();
}

ReportItemRepeater::~ReportItemRepeater()
{
	qfLogFuncFrame();
	QF_SAFE_DELETE(m_dataModel);
}

void ReportItemRepeater::setModel(QVariant m)
{
	if (m_model == m)
		return;
	m_model = m;
	QF_SAFE_DELETE(m_dataModel);
	emit modelChanged(m);

}

RepeaterModel *ReportItemRepeater::dataModel()
{
	if(!m_dataModel) {
		QVariant data = model();
		if(data.userType() == QVariant::String) {
			ReportItemRepeater *pr = parentRepeater();
			if(pr) {
				RepeaterModel *dm = pr->dataModel();
				if(dm) {
					data = dm->table(data.toString(), pr->currentIndex());
				}
			}
		}
		m_dataModel = RepeaterModel::createFromData(data, this);
	}
	return m_dataModel;
}

ReportItemRepeater *ReportItemRepeater::parentRepeater()
{
	ReportItemRepeater *ret = qf::core::findParent<ReportItemRepeater*>(this, false);
	return ret;
}

int ReportItemRepeater::itemsToPrintCount()
{
	int ret = Super::itemsToPrintCount();
	if(header())
		ret++;
	return ret;
}

ReportItem *ReportItemRepeater::itemToPrintAt(int ix)
{
	if(header()) {
		if(ix == 0)
			return header();
		else
			return Super::itemToPrintAt(ix - 1);
	}
	return Super::itemToPrintAt(ix);
}

QVariant ReportItemRepeater::data(const QString &field_name, int row_no, int role)
{
	QVariant ret;
	RepeaterModel *m = dataModel();
	if(m) {
		ret = m->data(field_name, row_no, (RepeaterModel::DataRole)role);
	}
	else {
		qfWarning() << "Repeater has not valid data model.";
	}
	return ret;
}

ReportItem::PrintResult ReportItemRepeater::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfLogFuncFrame() << this;
	if(isHeaderOnBreak()) {
		/// print everything except of detail again
		for(int i=0; i<itemsToPrintCount(); i++) {
			ReportItem *it = itemToPrintAt(i);
			//if(it->toDetail() == NULL)
			//	it->resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
		}
		indexToPrint = 0;
	}
	return Super::printMetaPaint(out, bounding_rect);
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

//==========================================================
//                                    ReportItemDetail
//==========================================================
ReportItemDetail::ReportItemDetail(ReportItem *parent)
	: ReportItemFrame(parent)
{
	//qfInfo() << QF_FUNC_NAME << "element id:" << element.attribute("id");
	f_currentRowNo = -1;
}

qfu::TreeTable ReportItemDetail::dataTable()
{
	ReportItemBand *b = parentBand();
	qfu::TreeTable data_table;
	if(b) {
		//qfDebug() << "band:" << b << "\ttable is null:" << b->dataTable().isNull();
		data_table = b->dataTable();
	}
	return data_table;
}

qfu::TreeTableRow ReportItemDetail::dataRow()
{
	return dataTable().row(currentRowNo());
}

void ReportItemDetail::resetIndexToPrintRecursively(bool including_para_texts)
{
	ReportItemFrame::resetIndexToPrintRecursively(including_para_texts);
}

ReportItem::PrintResult ReportItemDetail::printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect)
{
	qfLogFuncFrame() << this;
	//qfInfo() << QF_FUNC_NAME;
	bool design_mode = processor()->isDesignMode(); /// true znamena, zobraz prvni radek, i kdyz tam nejsou data.
	//qfInfo() << "design mode:" << design_mode;
	ReportItemBand *b = parentBand();
	qfu::TreeTable data_table;
	if(b) {
		data_table = b->dataTable();
		//qfInfo()<< element.attribute("id") << "band:" << b << "\ttable is null:" << b->dataTable().isNull() << "f_currentRowNo:" << f_currentRowNo << "of" << data_table.rowCount();
		if(!data_table.isNull()) {
			//design_view = false;
			if(currentRowNo() < 0) {
				/// kdyz neni f_dataRow, vezmi prvni radek dat
				//qfInfo() << "init f_currentRowNo to 0, element id:" << element.attribute("id");
				resetCurrentRowNo();
			}
		}
	}
	PrintResult res;
	if(!design_mode && (data_table.isNull() || dataRow().isNull())) {
		/// prazdnej detail vubec netiskni
		res.value = PrintOk;
		return res;
	}
	res = ReportItemFrame::printMetaPaint(out, bounding_rect);
	if(res.value == PrintOk) {
		if(b) {
			/// vezmi dalsi radek dat
			f_currentRowNo++;
			//qfInfo() << "vezmi dalsi radek dat element id:" << element.attribute("id") << "f_currentRowNo:" << f_currentRowNo;
			if(currentRowNo() < data_table.rowCount()) {
				resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
				res.flags |= FlagPrintAgain;
			}
			else {
				//qfInfo() << "detail setting f_currentRowNo to 0, element id:" << element.attribute("id");
				resetCurrentRowNo();
			}
		}
	}
	else {
		//qfWarning() << "detail print !OK, element id:" << element.attribute("id") << "f_currentRowNo:" << f_currentRowNo;
	}
	//res = checkPrintResult(res);
	qfDebug() << "\treturn:" << res.toString();
	return res;
}

#endif
