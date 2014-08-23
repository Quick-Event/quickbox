#include "dialogbuttonbox.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

DialogButtonBox::DialogButtonBox(QWidget * parent) :
	QDialogButtonBox(parent)
{
	setStandardButtons(Ok | Cancel);
}

DialogButtonBox::DialogButtonBox(QDialogButtonBox::StandardButtons buttons, QWidget *parent)
	: QDialogButtonBox(buttons, parent)
{
}
