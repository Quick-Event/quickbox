#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include "qmlwidgetsglobal.h"
#include "idatawidget.h"

#include <QPlainTextEdit>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT TextEdit : public QPlainTextEdit, public IDataWidget
{
	Q_OBJECT
	Q_PROPERTY(QVariant dataValue READ dataValue WRITE setDataValue NOTIFY dataValueChanged)
	Q_PROPERTY(QString dataId READ dataId WRITE setDataId)
private:
	using Super = QPlainTextEdit;
public:
	TextEdit(QWidget *parent = nullptr);
protected:
	QVariant dataValue() Q_DECL_OVERRIDE;
	void setDataValue(const QVariant &val) Q_DECL_OVERRIDE;
	Q_SIGNAL void dataValueChanged(const QVariant &new_val);
private:
	void onTextChanged();
private:
	bool m_blockTextChanged = false;
};

} // namespace qmlwidgets
} // namespace qf

#endif // TEXTEDIT_H
