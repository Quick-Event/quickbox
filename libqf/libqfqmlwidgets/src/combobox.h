#ifndef QF_QMLWIDGETS_COMBOBOX_H
#define QF_QMLWIDGETS_COMBOBOX_H

#include "qmlwidgetsglobal.h"
#include "idatawidget.h"

#include <qf/core/utils.h>

#include <QComboBox>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT ComboBox : public QComboBox, public IDataWidget
{
	Q_OBJECT
	Q_PROPERTY(QVariant dataValue READ dataValue WRITE setDataValue NOTIFY dataValueChanged)
	Q_PROPERTY(QString dataId READ dataId WRITE setDataId)
	Q_PROPERTY(bool valueRestrictedToItems READ isValueRestrictedToItems WRITE setValueRestrictedToItems NOTIFY valueRestrictedToItemsChanged)
private:
	typedef QComboBox Super;
public:
	ComboBox(QWidget *parent = nullptr);
	~ComboBox() Q_DECL_OVERRIDE;

	QF_PROPERTY_BOOL_IMPL2(v, V , alueRestrictedToItems, true)
protected:
	QVariant dataValue() Q_DECL_OVERRIDE;
	void setDataValue(const QVariant &val) Q_DECL_OVERRIDE;
	Q_SIGNAL void dataValueChanged(const QVariant &new_val);
};

}}

#endif // COMBOBOX_H
