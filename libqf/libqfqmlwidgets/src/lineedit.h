#ifndef QF_QMLWIDGETS_LINEEDIT_H
#define QF_QMLWIDGETS_LINEEDIT_H

#include "qmlwidgetsglobal.h"
#include "idatawidget.h"

#include <QLineEdit>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT LineEdit : public QLineEdit, public IDataWidget
{
	Q_OBJECT
	Q_ENUMS(EchoMode)
	Q_PROPERTY(QVariant dataValue READ dataValue WRITE setDataValue NOTIFY dataValueChanged)
	Q_PROPERTY(QString dataId READ dataId WRITE setDataId)
private:
	typedef QLineEdit Super;
public:
	explicit LineEdit(QWidget *parent = nullptr);
protected:
	QVariant dataValue() Q_DECL_OVERRIDE;
	void setDataValue(const QVariant &val) Q_DECL_OVERRIDE;
	Q_SIGNAL void dataValueChanged(const QVariant &new_val);
private:
	void onTextEdited(const QString &new_text);
};

}}

#endif // QF_QMLWIDGETS_LINEEDIT_H
