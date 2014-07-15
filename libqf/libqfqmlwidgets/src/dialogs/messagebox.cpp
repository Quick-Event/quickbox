#include "messagebox.h"

#include <qf/core/exception.h>

using namespace qf::qmlwidgets::dialogs;

MessageBox::MessageBox(QWidget *parent) :
	QMessageBox(parent)
{
}

void MessageBox::showException(QWidget *parent, const qf::core::Exception &exc)
{
	QString msg = exc.message();
	QMessageBox::critical(parent, tr("Exception"), msg);
}
