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

}}}
