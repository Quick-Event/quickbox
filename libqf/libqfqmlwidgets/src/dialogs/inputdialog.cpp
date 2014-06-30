#include "inputdialog.h"

#include <QApplication>

using namespace qf::qmlwidgets::dialogs;

InputDialog::InputDialog(QWidget *parent, Qt::WindowFlags flags) :
	QInputDialog(parent, flags)
{
}
