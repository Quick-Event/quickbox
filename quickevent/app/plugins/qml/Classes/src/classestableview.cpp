#include "Classes/classesplugin.h"
#include "classestableview.h"
#include "classeswidget.h"

#include <qf/core/assert.h>
#include <qf/core/sql/transaction.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/mainwindow.h>

#include <QInputDialog>

static Classes::ClassesPlugin* classesPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Classes::ClassesPlugin*>(fwk->plugin("Classes"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Classes plugin!");
	return plugin;
}

ClassesTableView::ClassesTableView(QWidget *parent)
	: Super(parent)
{

}

void ClassesTableView::insertRow()
{
	try {
		QString class_name = QInputDialog::getText(this, tr("Get text"), tr("New class name:"), QLineEdit::Normal);
		if(class_name.isEmpty())
			return;
		qf::core::sql::Transaction transaction;
		classesPlugin()->createClass(class_name);
		transaction.commit();
		reload();
	}
	catch (qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
	}
}

void ClassesTableView::removeSelectedRows()
{
	if(!qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Realy delete all selected classes? This action cannot be undone!"), false))
		return;
	try {
		qf::core::sql::Transaction transaction;
		for(int i : selectedRowsIndexes()) {
			int class_id = tableRow(i).value(idColumnName()).toInt();
			classesPlugin()->dropClass(class_id);
		}
		transaction.commit();
		reload();
	}
	catch (qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
	}
}
