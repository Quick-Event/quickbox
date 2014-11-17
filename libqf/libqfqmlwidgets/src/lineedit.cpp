#include "lineedit.h"

using namespace qf::qmlwidgets;

LineEdit::LineEdit(QWidget *parent) :
	QLineEdit(parent), IDataWidget(this)
{
	connect(this, &QLineEdit::textEdited, this, &LineEdit::onTextEdited);
}

QVariant LineEdit::dataValue()
{
	return text();
}

void LineEdit::setDataValue(const QVariant &val)
{
	QString new_text = val.toString();
	if(new_text != text()) {
		setText(new_text);
		saveDataValue();
		emit dataValueChanged(new_text);
	}
}

void LineEdit::onTextEdited(const QString &new_text)
{
	/// Unlike textChanged(), this signal is not emitted when the text is changed programmatically, for example, by calling setText().
	saveDataValue();
	emit dataValueChanged(new_text);
}
