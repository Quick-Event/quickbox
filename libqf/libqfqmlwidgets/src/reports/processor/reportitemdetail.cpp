#include "reportitemdetail.h"

#include "reportitemband.h"
#include "banddatamodel.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/string.h>

#include <qf/core/utils/timescope.h>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

//==========================================================
//                  ReportItemDetail
//==========================================================
ReportItemDetail::ReportItemDetail(ReportItem *parent)
	: Super(parent)
{
	qfLogFuncFrame();
}

ReportItemDetail::~ReportItemDetail()
{
	qfLogFuncFrame();
}

QVariant ReportItemDetail::data(int row_no, const QString &field_name, int role)
{
	qfLogFuncFrame() << "row_no:" << row_no << "field_name:" << field_name;
	QVariant ret;
	if(row_no >= 0) {
		ReportItemBand *band = parentBand();
		if(band) {
			BandDataModel *m = band->model();
			if(m) {
				if(row_no < 0)
					row_no = currentIndex();
				ret = m->data(row_no, field_name, (BandDataModel::DataRole)role);
			}
			else {
				qfWarning() << "Parent Band has not valid data model.";
			}
		}
		else {
			qfWarning() << "Detail without parent Band.";
		}
	}
	else {
		ret = QStringLiteral("N/A");
	}
	return ret;
}

QVariant ReportItemDetail::rowData(const QString &field_name, int role)
{
	return data(currentIndex(), field_name, role);
}

ReportItem::PrintResult ReportItemDetail::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfLogFuncFrame();
	QF_TIME_SCOPE("ReportItemDetail::printMetaPaint");
	ReportItemBand *band = qobject_cast<ReportItemBand*>(parent());
	BandDataModel *model = nullptr;
	if(band) {
		model = band->model();
		if(model) {
			if(currentIndex() < 0) {
				/// kdyz neni f_dataRow, vezmi prvni radek dat
				setCurrentIndex(0);
			}
		}
	}
	PrintResult res;
	/*--
	bool design_mode = processor()->isDesignMode(); /// true znamena, zobraz prvni radek, i kdyz tam nejsou data.
	//qfInfo() << "design mode:" << design_mode;
	if(!design_mode && (data_table.isNull() || dataRow().isNull())) {
		/// prazdnej detail vubec netiskni
		res.value = PrintOk;
		return res;
	}
	--*/
	res = Super::printMetaPaint(out, bounding_rect);
	if(res == PR_PrintedOk) {
		if(model) {
			/// take next data row
			int ix = currentIndex() + 1;
			qfDebug() << currentIndex() << "/" << model->rowCount();
			//qfDebug() << model->dump();
			if(ix < model->rowCount()) {
				QF_TIME_SCOPE("ReportItemDetail::printMetaPaint 3");
				setCurrentIndex(ix);
				{
					resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
				}
				res = PR_PrintAgainDetail;
			}
		}
	}
	return res;
}

#if 0

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
