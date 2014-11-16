#include "idatawidget.h"

#include "datacontroller.h"

#include <qf/core/model/datadocument.h>

namespace qfm = qf::core::model;
using namespace qf::qmlwidgets;

IDataWidget::IDataWidget(QWidget *data_widget)
	: m_dataWidget(data_widget)
{
}

IDataWidget::~IDataWidget()
{
}

void IDataWidget::loadDataValue(DataController *dc)
{
	if(dc != m_dataController)
		m_dataController = dc;
	qfm::DataDocument *doc = dataDocument();
	if(doc) {
		setDataValue(doc->value(dataId()));
	}
}

void IDataWidget::saveDataValue()
{
	qfm::DataDocument *doc = dataDocument();
	if(doc) {
		doc->setValue(dataId(), dataValue());
	}
}

void IDataWidget::finishDataValueEdits()
{
	qfLogFuncFrame();
}

QVariant IDataWidget::dataValue()
{
	return QVariant();
}

void IDataWidget::setDataValue(const QVariant &val)
{
	qfLogFuncFrame() << dataId() << "->" << val.toString();
}

qfm::DataDocument *IDataWidget::dataDocument()
{
	qfm::DataDocument *ret = nullptr;
	if(m_dataController) {
		ret = m_dataController->document();
	}
	return ret;
}

