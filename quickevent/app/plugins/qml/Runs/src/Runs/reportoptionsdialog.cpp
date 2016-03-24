#include "reportoptionsdialog.h"
#include "ui_reportoptionsdialog.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/string.h>
#include <qf/core/assert.h>

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

	//ui->edFilter->setText("h1%");
	//ui->grpClassFilter->setChecked(false);
	ui->btRegExp->setEnabled(eventPlugin()->sqlDriverName().endsWith(QLatin1String("PSQL"), Qt::CaseInsensitive));
}

ReportOptionsDialog::~ReportOptionsDialog()
{
	delete ui;
}

void ReportOptionsDialog::setClassNamesFilter(const QStringList &class_names)
{
	ui->grpClassFilter->setChecked(true);
	ui->btClassNames->setChecked(true);
	ui->chkClassFilterDoesntMatch->setChecked(false);
	ui->edFilter->setText(class_names.join(','));
}

ReportOptionsDialog::BreakType ReportOptionsDialog::breakType() const
{
	return static_cast<BreakType>(ui->cbxBreakAfterClassType->currentIndex());
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
			else if(ui->btClassNames->isChecked()) {
				qf::core::String s = filter_str;
				QStringList sl = s.splitAndTrim(',');
				QString filter_operator = ui->chkClassFilterDoesntMatch->isChecked()? "NOT IN": "IN";
				QString ret = "classes.name %1('%2')";
				ret = ret.arg(filter_operator).arg(sl.join("','"));
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
	ret["breakAfterEachClass"] = ui->chkPrintEveryClassOnNewPage->isChecked();
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
