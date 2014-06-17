#include "label.h"

using namespace qf::qmlwidgets;

Label::Label(QWidget *parent) :
	Super(parent)
{
}

void Label::setText(const QString &new_text)
{
	Super::setText(new_text);
	emit textChanged(new_text);
}
