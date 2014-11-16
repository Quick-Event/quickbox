#include "datadialogwidget.h"

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
	return m_dataController;
}

void DataDialogWidget::setDataController(qf::qmlwidgets::DataController *dc)
{
	m_dataController = dc;
}

void DataDialogWidget::load(const QVariant &id, qf::core::model::DataDocument::RecordEditMode mode)
{
	if(m_dataController) {
		if(m_dataController->document()) {
			m_dataController->document()->load(id, mode);
		}
	}
}

