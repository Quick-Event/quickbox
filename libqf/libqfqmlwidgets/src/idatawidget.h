#ifndef QF_QMLWIDGETS_IDATAWIDGET_H
#define QF_QMLWIDGETS_IDATAWIDGET_H

#include <QString>

class QWidget;

namespace qf {
namespace qmlwidgets {

class IDataWidget
{
public:
	IDataWidget(QWidget *data_widget);
	virtual ~IDataWidget();
public:
	QString dataId() const {return m_dataId;}
	void setDataId(const QString &id) {m_dataId = id;}

	QWidget* dataWidget() {return m_dataWidget;}
private:
	QWidget *m_dataWidget;
	QString m_dataId;
};

}}

#endif // QF_QMLWIDGETS_IDATAWIDGET_H
