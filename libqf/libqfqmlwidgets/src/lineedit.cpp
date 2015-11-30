#include "lineedit.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

LineEdit::LineEdit(QWidget *parent)
	: Super(parent)
	, IDataWidget(this)
{
	connect(this, &QLineEdit::textEdited, this, &LineEdit::onTextEdited);
}

QVariant LineEdit::dataValue()
{
	return text();
}

void LineEdit::setDataValue(const QVariant &val)
{
	qfLogFuncFrame() << dataId() << val;
	QString new_text = val.isNull()? QString(): val.toString();
	if(checkSetDataValueFirstTime()) {
		setText(new_text);
	}
	else {
		if(new_text != text()) {
			setText(new_text);
			saveDataValue();
			//qfInfo() << dataId() << "dataValueChanged:" << val;
			emit dataValueChanged(new_text);
		}
	}
}

void LineEdit::onTextEdited(const QString &new_text)
{
	/// Unlike textChanged(), this signal is not emitted when the text is changed programmatically, for example, by calling setText().
	saveDataValue();
	emit dataValueChanged(new_text);
}
