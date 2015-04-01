#ifndef QF_QMLWIDGETS_DATEEDIT_H
#define QF_QMLWIDGETS_DATEEDIT_H

#include "qmlwidgetsglobal.h"
#include "idatawidget.h"

#include <QDateEdit>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT DateEdit : public QDateEdit, public IDataWidget
{
	Q_OBJECT
	Q_PROPERTY(QVariant dataValue READ dataValue WRITE setDataValue NOTIFY dataValueChanged)
	Q_PROPERTY(QString dataId READ dataId WRITE setDataId)
private:
	typedef QDateEdit Super;
public:
	DateEdit(QWidget *parent = nullptr);
protected:
	QVariant dataValue() Q_DECL_OVERRIDE;
	void setDataValue(const QVariant &val) Q_DECL_OVERRIDE;
	Q_SIGNAL void dataValueChanged(const QVariant &new_val);
private:
	void onDateChanged(const QDate &new_date);
private:
	bool m_blockDateChanged = false;
};

} // namespace qmlwidgets
} // namespace qf

#endif // QF_QMLWIDGETS_DATEEDIT_H
