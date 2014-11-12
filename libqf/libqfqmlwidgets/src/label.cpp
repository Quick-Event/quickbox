#include "label.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

Label::Label(QWidget *parent) :
	Super(parent)
{
}

Qt::Alignment Label::horizontalAlignment() const
{
	return alignment() & Qt::AlignHorizontal_Mask;
}

void Label::setHorizontalAlignment(Qt::Alignment al)
{
	if(al != horizontalAlignment()) {
		int al1 = al & Qt::AlignHorizontal_Mask;
		int al2 = alignment() & Qt::AlignVertical_Mask;
		al = (Qt::Alignment) (al1 | al2);
		setAlignment(al);
		emit horizontalAlignmentChanged(al);
	}
}

Qt::Alignment Label::verticalAlignment() const
{
	return alignment() & Qt::AlignVertical_Mask;
}

void Label::setVerticalAlignment(Qt::Alignment al)
{
	if(al != verticalAlignment()) {
		int al1 = alignment() & Qt::AlignHorizontal_Mask;
		int al2 = al & Qt::AlignVertical_Mask;
		setAlignment((Qt::Alignment) (al1 | al2));
		emit verticalAlignmentChanged(al);
	}
}

void Label::setText(const QString &new_text)
{
	Super::setText(new_text);
	emit textChanged(new_text);
}
