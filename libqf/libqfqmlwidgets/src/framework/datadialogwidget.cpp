#include "datadialogwidget.h"
#include "../dialogs/dialog.h"

#include <qf/core/model/datadocument.h>
#include <qf/core/log.h>

namespace qfm = qf::core::model;
using namespace qf::qmlwidgets::framework;

DataDialogWidget::DataDialogWidget(QWidget *parent)
	: Super(parent)
{

}

DataDialogWidget::~DataDialogWidget()
{

}

qf::qmlwidgets::DataController *DataDialogWidget::dataController()
{
	if(!m_dataController) {
		m_dataController = new qf::qmlwidgets::DataController(this);
		m_dataController->setWidget(this);
	}
	return m_dataController;
}

void DataDialogWidget::setDataController(qf::qmlwidgets::DataController *dc)
{
	m_dataController = dc;
}

qf::core::model::DataDocument *DataDialogWidget::dataDocument(bool throw_exc)
{
	qf::qmlwidgets::DataController *dc = dataController();
	return dc->document(throw_exc);
}

bool DataDialogWidget::load(const QVariant &id, int mode)
{
	core::model::DataDocument *doc = dataDocument(!qf::core::Exception::Throw);
	if(doc) {
		//connect(doc, &qfm::DataDocument::saved, this, &DataDialogWidget::dataSaved, Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
		//qfInfo() << "============" << (bool)c;
		connect(doc, &qfm::DataDocument::saved, this, &DataDialogWidget::dataSaved, Qt::UniqueConnection);
		bool ok = doc->load(id, qf::core::model::DataDocument::RecordEditMode(mode));
		//qfInfo() << "emit ...";
		if(ok)
			emit recordEditModeChanged(mode);
		return ok;
	}
	return false;
}

bool DataDialogWidget::acceptDialogDone(int result)
{
	qfLogFuncFrame();
	bool ret = true;
	if(result == qf::qmlwidgets::dialogs::Dialog::ResultAccept) {
		auto mode = recordEditMode();
		if(mode == qf::core::model::DataDocument::ModeDelete) {
			ret = dropData();
		}
		else if(mode == qf::core::model::DataDocument::ModeEdit
				|| mode == qf::core::model::DataDocument::ModeInsert
				|| mode == qf::core::model::DataDocument::ModeCopy) {
			ret = saveData();
		}
	}
	if(ret)
		ret = Super::acceptDialogDone(result);
	return ret;
}

int DataDialogWidget::recordEditMode()
{
	int ret = -1;
	if(m_dataController && m_dataController->document(false)) {
		ret = m_dataController->document()->mode();
	}
	return ret;
}

bool DataDialogWidget::saveData()
{
	bool ret = false;
	DataController *dc = dataController();
	if(dc) {
		qfm::DataDocument *doc = dc->document(false);
		if(doc) {
			ret = doc->save();
		}
	}
	return ret;
}

bool DataDialogWidget::dropData()
{
	bool ret = false;
	DataController *dc = dataController();
	if(dc) {
		qfm::DataDocument *doc = dc->document(false);
		if(doc) {
			ret = doc->drop();
		}
	}
	return ret;
}

