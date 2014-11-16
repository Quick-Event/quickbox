#ifndef QF_QMLWIDGETS_DATACONTROLLER_H
#define QF_QMLWIDGETS_DATACONTROLLER_H

#include "qmlwidgetsglobal.h"
#include "idatawidget.h"

#include <qf/core/utils.h>
#include <qf/core/model/datadocument.h>

#include <QWidget>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT DataController : public QObject
{
	Q_OBJECT
	Q_PROPERTY(qf::core::model::DataDocument* document READ document WRITE setDocument NOTIFY documentChanged)
	Q_PROPERTY(QWidget* widget READ widget WRITE setWidget NOTIFY widgetChanged)
public:
	explicit DataController(QObject *parent = 0);
	~DataController() Q_DECL_OVERRIDE;

	qf::core::model::DataDocument* document() const { return m_document;}
	void setDocument(qf::core::model::DataDocument *doc)
	{
		if(m_document != doc) {
			m_document = doc;
			emit documentChanged(doc);
		}
	}
	Q_SIGNAL void documentChanged(qf::core::model::DataDocument *doc);

	QWidget* widget() const { return m_dataWidgetsParent;}
	void setWidget(QWidget *w)
	{
		if(m_dataWidgetsParent != w) {
			m_dataWidgetsParent = w;
			emit widgetChanged(w);
		}
	}
	Q_SIGNAL void widgetChanged(QWidget *doc);

protected:
	QList<IDataWidget*> dataWidgets();
	IDataWidget* dataWidget(const QString &data_id);
protected:
	qf::core::model::DataDocument *m_document = nullptr;
	QWidget *m_dataWidgetsParent = nullptr;
};

}}

#endif // QF_QMLWIDGETS_DATACONTROLLER_H
