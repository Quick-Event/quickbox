#include "siidedit.h"

namespace quickevent {
namespace gui {
namespace si {

SiIdEdit::SiIdEdit(QWidget *parent)
	: Super(parent)
{
	setMaximum(999999999);
	setMinimum(0);
}

void SiIdEdit::fixup(QString &input) const
{
	if (input.isEmpty())
		input = QString::fromLatin1("0");
	Super::fixup(input);
}

int SiIdEdit::valueFromText(const QString &text) const
{
	if (text.isEmpty())
		return 0;
	return text.toInt();
}

QString SiIdEdit::textFromValue(int value) const
{
	if (value == 0)
		return "";
	return QString::number(value);
}

}}}
