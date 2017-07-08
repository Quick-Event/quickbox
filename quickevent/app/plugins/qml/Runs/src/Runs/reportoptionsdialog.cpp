#include "reportoptionsdialog.h"
#include "ui_reportoptionsdialog.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/string.h>
#include <qf/core/assert.h>

#include <QSettings>
#include <QShowEvent>
#include <QTimer>

namespace Runs {

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Event");
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return qobject_cast<Event::EventPlugin*>(plugin);
}
namespace {
auto persistent_settings_path_prefix = QStringLiteral("ui/MainWindow/");
auto default_persistent_settings_id =  QStringLiteral("reportOptionsDialog");
}
ReportOptionsDialog::ReportOptionsDialog(QWidget *parent)
	: QDialog(parent)
	, qf::qmlwidgets::framework::IPersistentSettings(this)
	, ui(new Ui::ReportOptionsDialog)
{
	ui->setupUi(this);
	setPersistentSettingsId(default_persistent_settings_id);

	//ui->edFilter->setText("h1%");
	ui->grpStartOptions->setVisible(false);
	ui->btRegExp->setEnabled(eventPlugin()->sqlDriverName().endsWith(QLatin1String("PSQL"), Qt::CaseInsensitive));

	connect(ui->btSaveAsDefault, &QPushButton::clicked, [this]() {
		savePersistentSettings();
	});

	connect(this, &ReportOptionsDialog::startListOptionsVisibleChanged, ui->grpStartOptions, &QGroupBox::setVisible);
}

ReportOptionsDialog::~ReportOptionsDialog()
{
	delete ui;
}

QString ReportOptionsDialog::persistentSettingsPath()
{
	return persistent_settings_path_prefix + persistentSettingsId();
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
	const Options opts = options();
	return sqlWhereExpression(opts);
}

QString ReportOptionsDialog::sqlWhereExpression(const ReportOptionsDialog::Options &opts)
{
	if(opts.isUseClassFilter()) {
		QString filter_str = opts.classFilter();
		if(!filter_str.isEmpty()) {
			FilterType filter_type = (FilterType)opts.classFilterType();
			if(filter_type == FilterType::RegExp) {
				QString filter_operator = opts.isInvertClassFilter()? "!~*": "~*";
				QString ret = "classes.name %1 '%2'";
				ret = ret.arg(filter_operator).arg(filter_str);
				return ret;
			}
			else if(filter_type == FilterType::WildCard) {
				filter_str.replace('*', '%').replace('?', '_');
				QString filter_operator = opts.isInvertClassFilter()? "NOT LIKE": "LIKE";
				QString ret = "classes.name %1 '%2'";
				ret = ret.arg(filter_operator).arg(filter_str);
				return ret;
			}
			else if(filter_type == FilterType::ClassName) {
				qf::core::String s = filter_str;
				QStringList sl = s.splitAndTrim(',');
				QString filter_operator = opts.isInvertClassFilter()? "NOT IN": "IN";
				QString ret = "classes.name %1('%2')";
				ret = ret.arg(filter_operator).arg(sl.join("','"));
				return ret;
			}
		}
	}
	return QString();
}

void ReportOptionsDialog::showEvent(QShowEvent *event)
{
	Super::showEvent(event);
	if(event->spontaneous())
		return;
	ui->grpClassFilter->setVisible(isClassFilterVisible());
}
/*
int ReportOptionsDialog::exec()
{
	loadPersistentSettings();
	int result = Super::exec();
	return result;
}

int ReportOptionsDialog::exec(const ReportOptionsDialog::Options &options)
{
	int result = Super::exec();
	return result;
}
*/
void ReportOptionsDialog::setOptions(const ReportOptionsDialog::Options &options)
{
	ui->cbxBreakAfterClassType->setCurrentIndex(options.breakType());
	ui->grpClassFilter->setChecked(options.isUseClassFilter());
	ui->chkClassFilterDoesntMatch->setChecked(options.isInvertClassFilter());
	ui->edFilter->setText(options.classFilter());
	FilterType filter_type = (FilterType)options.classFilterType();
	ui->btWildCard->setChecked(filter_type == FilterType::WildCard);
	ui->btRegExp->setChecked(filter_type == FilterType::RegExp);
	ui->btClassNames->setChecked(filter_type == FilterType::ClassName);
	ui->chkStartOpts_PrintVacants->setChecked(options.isStartListPrintVacants());
	ui->chkStartOpts_PrintStartNumbers->setChecked(options.isStartListPrintStartNumbers());
}

ReportOptionsDialog::Options ReportOptionsDialog::options() const
{
	Options opts;
	opts.setBreakType(ui->cbxBreakAfterClassType->currentIndex());
	opts.setUseClassFilter(ui->grpClassFilter->isChecked());
	opts.setInvertClassFilter(ui->chkClassFilterDoesntMatch->isChecked());
	opts.setClassFilter(ui->edFilter->text());
	FilterType filter_type =  ui->btWildCard->isChecked()? FilterType::WildCard: ui->btRegExp->isChecked()? FilterType::RegExp: FilterType::ClassName;
	opts.setClassFilterType((int)filter_type);
	opts.setStartListPrintVacants(isStartListPrintVacants());
	opts.setStartListPrintStartNumbers(isStartListPrintStartNumbers());
	return opts;
}

ReportOptionsDialog::Options ReportOptionsDialog::savedOptions()
{
	QSettings settings;
	QVariantMap m = settings.value(persistent_settings_path_prefix + default_persistent_settings_id).toMap();
	//qfInfo() << persistentSettingsPath() << m;
	return Options(m);
}

void ReportOptionsDialog::loadPersistentSettings()
{
	if(persistentSettingsId().isEmpty())
		return;
	QSettings settings;
	QVariantMap m = settings.value(persistentSettingsPath()).toMap();
	//qfInfo() << persistentSettingsPath() << m;
	Options opts(m);
	setOptions(opts);
}

void ReportOptionsDialog::savePersistentSettings()
{
	if(persistentSettingsId().isEmpty())
		return;

	Options opts = options();
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
