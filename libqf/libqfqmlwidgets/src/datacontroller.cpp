#include "datacontroller.h"

#include "idatawidget.h"

#include <QWidget>

using namespace qf::qmlwidgets;

DataController::DataController(QObject *parent)
	: QObject(parent)
{
	connect(this, SIGNAL(widgetChanged(QWidget*)), this, SLOT(clearDataWidgetsCache()));
}

DataController::~DataController()
{

}

void DataController::setDocument(qf::core::model::DataDocument *doc)
{
	if(m_document != doc) {
		m_document = doc;
		connect(doc, &qf::core::model::DataDocument::loaded, this, &DataController::documentLoaded);
		connect(doc, &qf::core::model::DataDocument::valueChanged, this, &DataController::documentValueChanged);
		connect(doc, &qf::core::model::DataDocument::aboutToSave, this, &DataController::documentAboutToSave);
		emit documentChanged(doc);
	}
}

QList<IDataWidget *> DataController::dataWidgets()
{
	if(m_dataWidgets.isEmpty()) {
		QList<QWidget *> lst = m_dataWidgetsParent->findChildren<QWidget*>();
		for(auto w : lst) {
			IDataWidget *dw = dynamic_cast<IDataWidget*>(w);
			if(dw)
				m_dataWidgets << dw;
		}
	}
	return m_dataWidgets;
}

IDataWidget *DataController::dataWidget(const QString &data_id)
{
	IDataWidget *ret = nullptr;
	for(auto dw : dataWidgets()) {
		if(data_id.compare(dw->dataId(), Qt::CaseInsensitive) == 0) {
			ret = dw;
			break;
		}
	}
	return ret;
}

void DataController::clearDataWidgetsCache()
{
	m_dataWidgets.clear();
}

void DataController::documentLoaded()
{
	for(auto dw : dataWidgets()) {
		dw->loadDataValue(this);
	}
}

void DataController::documentValueChanged(const QString &data_id, const QVariant &old_val, const QVariant &new_val)
{
	Q_UNUSED(old_val);
	Q_UNUSED(new_val);
	IDataWidget *dw = dataWidget(data_id);
	if(!dw) {
		// can happen, every data_id might not be connected to data widget
		//qfWarning() << "Cannot find data widget for data id:" << data_id;
	}
	else {
		dw->loadDataValue(this);
	}
}

void DataController::documentAboutToSave()
{
	for(auto dw : dataWidgets()) {
		dw->finishDataValueEdits();
	}
}


