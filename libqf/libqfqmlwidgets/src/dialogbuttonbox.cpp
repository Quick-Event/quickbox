#include "dialogbuttonbox.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

DialogButtonBox::DialogButtonBox(QWidget * parent) :
	QDialogButtonBox(parent)
{
	setStandardButtons(Ok | Cancel);
}
