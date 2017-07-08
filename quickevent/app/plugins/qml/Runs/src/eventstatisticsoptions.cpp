#include "eventstatisticsoptions.h"
#include "ui_eventstatisticsoptions.h"

#include <qf/core/log.h>

#include <QSettings>

EventStatisticsOptions::EventStatisticsOptions(QWidget *parent)
	: QDialog(parent)
	, qf::qmlwidgets::framework::IPersistentSettings(this)
	, ui(new Ui::EventStatisticsOptions)
{
	ui->setupUi(this);
	setPersistentSettingsId("eventStatisticsOptions");
}

EventStatisticsOptions::~EventStatisticsOptions()
{
	delete ui;
}

QString EventStatisticsOptions::staticPersistentSettingsPath()
{
	return QStringLiteral("ui/MainWindow/Runs/eventStatistics/eventStatisticsOptions");
}

int EventStatisticsOptions::exec()
{
	loadPersistentSettings();
	int result = Super::exec();
	if(result == QDialog::Accepted)
		savePersistentSettings();
	return result;
}

void EventStatisticsOptions::loadPersistentSettings()
{
	if(persistentSettingsId().isEmpty())
		return;
	qfDebug() << "persistentSettingsPath:" << persistentSettingsPath();
	QSettings settings;
	QVariantMap m = settings.value(persistentSettingsPath()).toMap();
	Options opts(m);
	ui->autoRefreshSec->setValue(opts.autoRefreshSec());
	ui->autoPrintNewRunners->setValue(opts.autoPrintNewRunners());
	ui->autoPrintNewMin->setValue(opts.autoPrintNewMin());
}

void EventStatisticsOptions::savePersistentSettings()
{
	if(persistentSettingsId().isEmpty())
		return;

	Options opts;
	opts.setAutoRefreshSec(ui->autoRefreshSec->value());
	opts.setAutoPrintNewRunners(ui->autoPrintNewRunners->value());
	opts.setAutoPrintNewMin(ui->autoPrintNewMin->value());

	QSettings settings;
	settings.setValue(persistentSettingsPath(), opts);
}
