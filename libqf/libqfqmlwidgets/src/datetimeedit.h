#pragma once

#include "qmlwidgetsglobal.h"
#include "idatawidget.h"

#include <QDateEdit>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT DateTimeEdit : public QDateTimeEdit, public IDataWidget
{
	Q_OBJECT
	Q_PROPERTY(QVariant dataValue READ dataValue WRITE setDataValue NOTIFY dataValueChanged)
	Q_PROPERTY(QString dataId READ dataId WRITE setDataId)
private:
	using Super = QDateTimeEdit;
public:
	DateTimeEdit(QWidget *parent = nullptr);
protected:
	QVariant dataValue() Q_DECL_OVERRIDE;
	void setDataValue(const QVariant &val) Q_DECL_OVERRIDE;
	Q_SIGNAL void dataValueChanged(const QVariant &new_val);
private:
	void onDateTimeChanged(const QDateTime &new_time);
private:
	bool m_blockDateTimeChanged = false;
};

} // namespace qmlwidgets
} // namespace qf

