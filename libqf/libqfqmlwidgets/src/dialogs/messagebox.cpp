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

void MessageBox::showError(QWidget *parent, const QString &message)
{
	QMessageBox::critical(parent, tr("Error"), message);
}

void MessageBox::showInfo(QWidget *parent, const QString &message)
{
	QMessageBox::information(parent, tr("Information"), message);
}
