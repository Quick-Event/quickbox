#include "reportoptionsdialog.h"
#include "ui_reportoptionsdialog.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/string.h>
#include <qf/core/assert.h>

#include <QSettings>
#include <QTimer>

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
	, qf::qmlwidgets::framework::IPersistentSettings(this)
	, ui(new Ui::ReportOptionsDialog)
{
	ui->setupUi(this);

	//ui->edFilter->setText("h1%");
	ui->grpStartOptions->setVisible(false);
	ui->btRegExp->setEnabled(eventPlugin()->sqlDriverName().endsWith(QLatin1String("PSQL"), Qt::CaseInsensitive));

	connect(this, &ReportOptionsDialog::startListOptionsVisibleChanged, ui->grpStartOptions, &QGroupBox::setVisible);
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

bool ReportOptionsDialog::isStartListPrintVacants() const
{
	return ui->chkStartOpts_PrintVacants->isChecked();
}

bool ReportOptionsDialog::isStartListPrintStartNumbers() const
{
	return ui->chkStartOpts_PrintStartNumbers->isChecked();
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

int ReportOptionsDialog::exec()
{
	loadPersistentSettings();
	QTimer::singleShot(0, [this]() {
		ui->grpClassFilter->setVisible(isClassFilterVisible());
	});
	int result = Super::exec();
	if(result == QDialog::Accepted)
		savePersistentSettings();
	return result;
}

void ReportOptionsDialog::loadPersistentSettings()
{
	if(persistentSettingsId().isEmpty())
		return;
	QSettings settings;
	QVariantMap m = settings.value(persistentSettingsPath()).toMap();
	Options opts(m);
	ui->cbxBreakAfterClassType->setCurrentIndex(opts.breakType());
	ui->grpClassFilter->setChecked(opts.isUseClassFilter());
	ui->chkClassFilterDoesntMatch->setChecked(opts.isInvertClassFilter());
	ui->edFilter->setText(opts.classFilter());
	FilterType filter_type = (FilterType)opts.classFilterType();
	ui->btWildCard->setChecked(filter_type == FilterType::WildCard);
	ui->btRegExp->setChecked(filter_type == FilterType::RegExp);
	ui->btClassNames->setChecked(filter_type == FilterType::ClassName);
	ui->chkStartOpts_PrintVacants->setChecked(opts.isStartListPrintVacants());
	ui->chkStartOpts_PrintStartNumbers->setChecked(opts.isStartListPrintStartNumbers());
}

void ReportOptionsDialog::savePersistentSettings()
{
	if(persistentSettingsId().isEmpty())
		return;

	Options opts;
	opts.setBreakType(ui->cbxBreakAfterClassType->currentIndex());
	opts.setUseClassFilter(ui->grpClassFilter->isChecked());
	opts.setInvertClassFilter(ui->chkClassFilterDoesntMatch->isChecked());
	opts.setClassFilter(ui->edFilter->text());
	FilterType filter_type =  ui->btWildCard->isChecked()? FilterType::WildCard: ui->btRegExp->isChecked()? FilterType::RegExp: FilterType::ClassName;
	opts.setClassFilterType((int)filter_type);
	opts.setStartListPrintVacants(isStartListPrintVacants());
	opts.setStartListPrintStartNumbers(isStartListPrintStartNumbers());

	QSettings settings;
	settings.setValue(persistentSettingsPath(), opts);
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
