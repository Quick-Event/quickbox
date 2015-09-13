#include "reportitemband.h"

#include "reportprocessor.h"
#include "reportitemdetail.h"
#include "reportitempara.h"
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

void ReportItemDetail::resetCurrentIndex()
{
	m_currentIndex = -1;
}

void ReportItemBand::setModelData(QVariant d)
{
	if (m_data == d)
		return;
	m_data = d;
	QF_SAFE_DELETE(m_model);
	emit modelDataChanged(d);

}

BandDataModel *ReportItemBand::model()
{
	if(!m_model || !m_model->isDataValid()) {
		ReportItemDetail *parent_detail = nullptr;
		QVariant dta = modelData();
		if(!dta.isValid() || dta.userType() == QVariant::String) {
			QString data_key = dta.toString();
			ReportItemBand *pr = parentBand();
			//qfInfo() << data_key << pr;
			if(pr) {
				BandDataModel *dm = pr->model();
				parent_detail = pr->detail();
				if(dm && parent_detail) {
					dta = dm->table(parent_detail->currentIndex(), data_key);
				}
			}
			else {
				/// take data from report processor
				ReportProcessor *proc = processor(!qf::core::Exception::Throw);
				if(proc) {
					dta = proc->data(data_key);
				}
			}
		}
		QF_SAFE_DELETE(m_model);
		m_model = BandDataModel::createFromData(dta, this);
		//qfError() << dta;
		QF_ASSERT(m_model != nullptr, "Bad data!", return m_model);
		if(parent_detail)
			connect(parent_detail, &ReportItemDetail::currentIndexChanged, m_model, &BandDataModel::invalidateData);
		//emit modelLoadedChanged(true);
	}
	return m_model;
}
/*
bool ReportItemBand::modelLoaded() const
{
	return (m_model != nullptr && m_model->isDataValid());
}
*/
QVariant ReportItemBand::data(const QString &field_name, int role)
{
	qfLogFuncFrame() << "field_name:" << field_name;
	QVariant ret;
	BandDataModel *m = model();
	if(m) {
		ret = m->tableData(field_name, (BandDataModel::DataRole)role);
	}
	else {
		qfWarning() << "Band has not valid data model.";
	}
	return ret;
}

ReportItem::PrintResult ReportItemBand::printMetaPaint(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect)
{
	qfLogFuncFrame() << this;
	if(isCreateFromData() && !detail()) {
		createChildItemsFromData();
	}
	if(isHeaderOnBreak()) {
		/// print everything except of detail again
		ReportItemDetail *it_det = detail();
		if(it_det)
			it_det->resetIndexToPrintRecursively(ReportItem::IncludingParaTexts);
		m_indexToPrint = 0;
	}
	auto *m = model(); // load model before rendering
	PrintResult res = PR_PrintedOk;
	if(m->rowCount() > 0) {
		res = Super::printMetaPaint(out, bounding_rect);
	}
	qfDebug() << "\tRETURN:" << res.toString();
	return res;
}

void ReportItemBand::resetIndexToPrintRecursively(bool including_para_texts)
{
	qfLogFuncFrame() << objectName();// << currentIndex();
	Super::resetIndexToPrintRecursively(including_para_texts);
	//dataTableLoaded = false;
	ReportItemDetail *det = detail();
	if(det) {
		det->resetCurrentIndex();
	}
}

bool ReportItemBand::canBreak()
{
	bool ret = Super::canBreak();
	if(ret) {
		if(keepFirst() > 0) {
			ReportItemDetail *det = detail();
			if(det) {
				ret = (det->currentIndex() > keepFirst());
			}
		}
	}
	return ret;
}

ReportItemDetail *ReportItemBand::detail()
{
	ReportItemDetail *ret = findChild<ReportItemDetail*>(QString(), Qt::FindDirectChildrenOnly);
	return ret;
}

void ReportItemBand::createChildItemsFromData()
{
	BandDataModel *mod = model();
	qfLogFuncFrame() << "model:" << mod;
	if(mod) {
		style::Brush *brush_lightgray = new style::Brush(this);
		{
			style::Color *c = new style::Color(brush_lightgray);
			c->setDefinition(QColor(Qt::lightGray));
			brush_lightgray->setColor(c);
		}
		ReportItemFrame *it_header_frm = new ReportItemFrame(this);
		{
			it_header_frm->setWidth("%");
			it_header_frm->setLayout(ReportItemFrame::LayoutHorizontal);
			it_header_frm->setFill(brush_lightgray);
			//it_header_frm->setHinset(3);
			//it_header_frm->setVinset(3);
		}
		addItem(it_header_frm);
		style::Pen *pen_black1 = new style::Pen(this);
		{
			pen_black1->setWidth(1);
			style::Color *c = new style::Color(pen_black1);
			c->setDefinition(QColor(Qt::black));
			pen_black1->setColor(c);
		}
		int col_cnt = mod->columnCount();
		for(int i=0; i<col_cnt; i++) {
			ReportItemPara *it_para = new ReportItemPara(it_header_frm);
			it_para->setBorder(pen_black1);
			it_para->setWidth(mod->headerData(i, Qt::SizeHintRole));
			it_para->setText(mod->headerData(i).toString());
			it_para->setHinset(1);
			it_header_frm->addItem(it_para);
		}
		ReportItemDetail *it_det = new ReportItemDetail(this);
		{
			it_det->setWidth("%");
			it_det->setLayout(ReportItemFrame::LayoutHorizontal);
			it_det->setExpandChildrenFrames(true);
		}
		addItem(it_det);
		for(int i=0; i<col_cnt; i++) {
			ReportItemPara *it_para = new ReportItemPara(it_det);
			it_para->setBorder(pen_black1);
			it_para->setWidth(mod->headerData(i, Qt::SizeHintRole));
			it_para->setHinset(1);
			auto fn = [mod, it_det, i]()->QString {
				QString ret = mod->data(it_det->currentIndex(), i).toString();
				return ret;
			};
			it_para->setGetTextCppFunction(fn);
			it_det->addItem(it_para);
		}
	}
	//dumpObjectTree();
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
