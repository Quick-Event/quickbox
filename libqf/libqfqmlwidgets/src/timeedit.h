#ifndef QF_QMLWIDGETS_TIMEEDIT_H
#define QF_QMLWIDGETS_TIMEEDIT_H

#include "qmlwidgetsglobal.h"
#include "idatawidget.h"

#include <QTimeEdit>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT TimeEdit : public QTimeEdit, public IDataWidget
{
	Q_OBJECT
	Q_PROPERTY(QVariant dataValue READ dataValue WRITE setDataValue NOTIFY dataValueChanged)
	Q_PROPERTY(QString dataId READ dataId WRITE setDataId)
private:
	typedef QTimeEdit Super;
public:
	TimeEdit(QWidget *parent = nullptr);
protected:
	QVariant dataValue() Q_DECL_OVERRIDE;
	void setDataValue(const QVariant &val) Q_DECL_OVERRIDE;
	Q_SIGNAL void dataValueChanged(const QVariant &new_val);
private:
	void onTimeChanged(const QTime &new_time);
private:
	bool m_blockTimeChanged = false;
};

}}

#endif
