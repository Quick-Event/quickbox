#include "coresettingspage.h"
#include "ui_coresettingswidget.h"

#include <QSettings>

CoreSettingsPage::CoreSettingsPage(QWidget *parent) :
	Super(parent),
	ui(new Ui::CoreSettingsWidget)
{
	m_persistentId = "Core/settings";
	m_caption = tr("Core");
	ui->setupUi(this);
}

CoreSettingsPage::~CoreSettingsPage()
{
	delete ui;
}

void CoreSettingsPage::on_btSelectCustomReportsDirectory_clicked()
{

}


void CoreSettingsPage::on_btExportReportDefinitions_clicked()
{

}

static const auto KEY_customReportsDirectory = QStringLiteral("customReportsDirectory");

void CoreSettingsPage::load()
{
	QSettings settings;
	settings.beginGroup(m_persistentId);
	ui->edSelectCustomReportsDirectory->setText(settings.value(KEY_customReportsDirectory).toString());
}

void CoreSettingsPage::save()
{
	QSettings settings;
	settings.beginGroup(m_persistentId);
	settings.setValue(KEY_customReportsDirectory, ui->edSelectCustomReportsDirectory->text().trimmed());
}

