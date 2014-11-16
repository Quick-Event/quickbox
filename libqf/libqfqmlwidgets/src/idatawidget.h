#ifndef QF_QMLWIDGETS_IDATAWIDGET_H
#define QF_QMLWIDGETS_IDATAWIDGET_H

#include "qmlwidgetsglobal.h"

#include <QString>
#include <QPointer>

class QWidget;

namespace qf {

namespace core {
namespace model {
class DataDocument;
}
}

namespace qmlwidgets {

class DataController;
class DataDocument;

class QFQMLWIDGETS_DECL_EXPORT IDataWidget
{
public:
	IDataWidget(QWidget *data_widget);
	virtual ~IDataWidget();
public:
	QString dataId() const {return m_dataId;}
	void setDataId(const QString &id) {m_dataId = id;}

	virtual void loadDataValue(DataController *dc);
	virtual void saveDataValue();
	/// called before document is saved to close current widget editor and save it's data
	virtual void finishDataValueEdits();
	virtual QVariant dataValue();
	virtual void setDataValue(const QVariant &val);

	QWidget* dataWidget() {return m_dataWidget;}
protected:
	qf::core::model::DataDocument* dataDocument();
private:
	QPointer<DataController> m_dataController;
	QWidget *m_dataWidget;
	QString m_dataId;
};

}}

#endif // QF_QMLWIDGETS_IDATAWIDGET_H
