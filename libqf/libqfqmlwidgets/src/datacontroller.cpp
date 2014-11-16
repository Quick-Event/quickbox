#include "datacontroller.h"

#include <QWidget>

using namespace qf::qmlwidgets;

DataController::DataController(QObject *parent)
	: QObject(parent)
{

}

DataController::~DataController()
{

}

QList<IDataWidget *> DataController::dataWidgets()
{
	QList<IDataWidget *> ret;
	QList<QWidget *> lst = m_dataWidgetsParent->findChildren<QWidget*>();
	for(auto w : lst) {
		IDataWidget *dw = dynamic_cast<IDataWidget*>(w);
		if(dw)
			ret << dw;
	}
	return ret;
}

static IDataWidget *dataWidget_helper(QWidget *parent, const QString &data_id)
{
	IDataWidget *ret = nullptr;
	if(parent) {
		IDataWidget *dw = dynamic_cast<IDataWidget*>(parent);
		if(dw && data_id.compare(dw->dataId(), Qt::CaseInsensitive) == 0) {
			ret = dw;
		}
		else {
			for(auto o : parent->children()) {
				QWidget *w = qobject_cast<QWidget*>(o);
				if(w) {
					ret = dataWidget_helper(w, data_id);
					if(ret)
						break;
				}
			}
		}
	}
	return ret;
}

IDataWidget *DataController::dataWidget(const QString &data_id)
{
	return dataWidget_helper(m_dataWidgetsParent, data_id);
}


