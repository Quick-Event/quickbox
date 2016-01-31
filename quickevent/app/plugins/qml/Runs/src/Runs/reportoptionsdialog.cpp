#include "reportoptionsdialog.h"
#include "ui_reportoptionsdialog.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

namespace Runs {

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Event");
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return qobject_cast<Event::EventPlugin*>(plugin);
}

ReportOptionsDialog::ReportOptionsDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ReportOptionsDialog)
{
	ui->setupUi(this);

	ui->edFilter->setText("h1%");
	ui->chkPrintEveryClassOnNewPage->setChecked(true);
	//ui->grpClassFilter->setChecked(false);
	ui->btRegExp->setEnabled(eventPlugin()->sqlDriverName().endsWith(QLatin1String("PSQL"), Qt::CaseInsensitive));
}

ReportOptionsDialog::~ReportOptionsDialog()
{
	delete ui;
}

bool ReportOptionsDialog::isEveryClassOnNewPage() const
{
	return ui->chkPrintEveryClassOnNewPage->isChecked();
}

QString ReportOptionsDialog::sqlWhereExpression() const
{
	if(ui->grpClassFilter->isChecked()) {
		QString filter_str = ui->edFilter->text();
		if(!filter_str.isEmpty()) {
			if(ui->btRegExp->isChecked()) {
				QString filter_operator = ui->chkClassFilterDoesntMatch->isChecked()? "!~*": "~*";
				QString ret = "classes.name %1 '%2'";
				ret = ret.arg(filter_operator).arg(filter_str);
				return ret;
			}
			else if(ui->btWildCard->isChecked()) {
				filter_str.replace('*', '%').replace('?', '_');
				QString filter_operator = ui->chkClassFilterDoesntMatch->isChecked()? "NOT LIKE": "LIKE";
				QString ret = "classes.name %1 '%2'";
				ret = ret.arg(filter_operator).arg(filter_str);
				return ret;
			}
		}
	}
	return QString();
}
/*
QVariantMap ReportOptionsDialog::optionsToMap() const
{
	QVariantMap ret;
	ret["everyClassOnNewPage"] = ui->chkPrintEveryClassOnNewPage->isChecked();
	if(ui->grpClassFilter->isChecked()) {
		QVariantMap m;
		m["filterDoesnMatch"] = ui->chkClassFilterDoesntMatch->isChecked();
		m["filterType"] = ui->btRegExp->isChecked()? "regExp": "wildCard";
		//m["sqlWhere"] = sqlWhereExpression();
		ret["classFilter"] = m;
	}
	return ret;
}
*/
} // namespace Runs
