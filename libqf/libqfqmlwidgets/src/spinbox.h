#ifndef QF_QMLWIDGETS_SPINBOX_H
#define QF_QMLWIDGETS_SPINBOX_H

#include "qmlwidgetsglobal.h"
#include "idatawidget.h"

#include <QSpinBox>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT SpinBox : public QSpinBox, public IDataWidget
{
	Q_OBJECT
	Q_PROPERTY(QVariant dataValue READ dataValue WRITE setDataValue NOTIFY dataValueChanged)
	Q_PROPERTY(QString dataId READ dataId WRITE setDataId)
private:
	typedef QSpinBox Super;
public:
	explicit SpinBox(QWidget *parent = nullptr);
	~SpinBox() Q_DECL_OVERRIDE {}
protected:
	QVariant dataValue() Q_DECL_OVERRIDE;
	void setDataValue(const QVariant &val) Q_DECL_OVERRIDE;
	Q_SIGNAL void dataValueChanged(const QVariant &new_val);
private:
	Q_SLOT void onValueChanged(int n);
};

}}

#endif // QF_QMLWIDGETS_SPINBOX_H
