#include "reportssettingspage.h"
#include "ui_reportssettingspage.h"
#include "../reportssettings.h"

#include <qf/core/log.h>
#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <QDirIterator>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

namespace Core {

ReportsSettingsPage::ReportsSettingsPage(QWidget *parent) :
	Super(parent),
	ui(new Ui::ReportsSettingsPage)
{
	m_caption = tr("Reports");
	ui->setupUi(this);

	connect(ui->btSetDefaultReportsDir, &QAbstractButton::clicked, this, [this]() {
		ui->edReportsDirectory->setText(qf::qmlwidgets::framework::Plugin::defaultReportsDir());
	});
}

ReportsSettingsPage::~ReportsSettingsPage()
{
	delete ui;
}

void ReportsSettingsPage::setReportsDirectory(const QString dir)
{
	auto old_dir = ui->edReportsDirectory->text();
	auto new_dir = dir;
	if(dir.isEmpty()) {
		new_dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
														 old_dir,
														 QFileDialog::ShowDirsOnly);
	}
	if(new_dir.isEmpty())
		return;
	ui->edReportsDirectory->setText(new_dir);
	//if(new_dir != old_dir)
	//	QMessageBox::information(this, tr("Information"), tr("Custom reports directory changes will be applied after application restart."));
}

void ReportsSettingsPage::on_btSelectCustomReportsDirectory_clicked()
{
	setReportsDirectory({});
}

static const auto KEY_reportsDirectory = QStringLiteral("reportsDirectory");
//static const auto KEY_exportReportDefinitionsDir = QStringLiteral("exportReportDefinitionsDir");

void ReportsSettingsPage::load()
{
	auto dir = reportsDirectory();
	ui->edReportsDirectory->setText(dir);
}

void ReportsSettingsPage::save()
{
	ReportsSettings settings;
	auto dir = ui->edReportsDirectory->text().trimmed();
	settings.setReportsDirectory(dir);
	qf::qmlwidgets::framework::Plugin::setReportsDir(dir);
}

QString ReportsSettingsPage::reportsDirectory() const
{
	ReportsSettings settings;
	auto dir = settings.reportsDirectory();
	if(dir.isEmpty())
		dir = qf::qmlwidgets::framework::Plugin::reportsDir();
	return dir;
}

}

