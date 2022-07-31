#include "reportssettingspage.h"
#include "ui_reportssettingspage.h"
#include "../coreplugin.h"
#include "../reportssettings.h"

#include <qf/core/log.h>
#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/framework/mainwindow.h>

#include <QDirIterator>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

namespace Core {

ReportsSettingsPage::ReportsSettingsPage(QWidget *parent) :
	Super(parent),
	ui(new Ui::ReportsSettingsPage)
{
	//auto *plugin = qf::qmlwidgets::framework::getPlugin<Core::CorePlugin>();
	//m_settingsDir = plugin->settingsDir() + "/reports";
	m_caption = tr("Reports");
	ui->setupUi(this);
}

ReportsSettingsPage::~ReportsSettingsPage()
{
	delete ui;
}

void ReportsSettingsPage::setCustomReportsDirectory(const QString dir)
{
	auto old_dir = ui->edCustomReportsDirectory->text();
	auto new_dir = dir;
	if(dir.isEmpty()) {
		new_dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
														 old_dir,
														 QFileDialog::ShowDirsOnly);
	}
	if(new_dir.isEmpty())
		return;
	ui->edCustomReportsDirectory->setText(new_dir);
	//if(new_dir != old_dir)
	//	QMessageBox::information(this, tr("Information"), tr("Custom reports directory changes will be applied after application restart."));
}

void ReportsSettingsPage::on_btSelectCustomReportsDirectory_clicked()
{
	setCustomReportsDirectory({});
}

void ReportsSettingsPage::on_btCreateCustomReportsDirectory_clicked()
{
	auto old_dir = ui->edCustomReportsDirectory->text();
	QString custom_reports_dir_name = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
													 old_dir,
													 QFileDialog::ShowDirsOnly);
	if(custom_reports_dir_name.isEmpty())
		return;
	//m_exportReportDefinitionsDir = export_dir_name;
	QDir export_dir(custom_reports_dir_name);
	const auto PREFIX = QStringLiteral(":/quickevent/");
	const auto REPORTS = QStringLiteral("/reports");
	const auto SHARED = QStringLiteral("shared/");
	QDirIterator it(":/", QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString name = it.next();
		if(!name.startsWith(PREFIX))
			continue;
		name = name.mid(PREFIX.length());
		if(name.startsWith(SHARED))
			continue;
		if(!name.contains(REPORTS))
			continue;
		QString export_path = export_dir.absolutePath() + '/' + name;
		QFileInfo fi = it.fileInfo();
		if(fi.isDir()) {
			qfInfo()<< "Creating dir:" << export_path;
			if(!export_dir.mkpath(name))
				qfError() << "cannot create dir:" << export_path;
		}
		else {
			qfInfo()<< "Copying file:" << export_path;
			if(QFile::copy(it.filePath(), export_path))
				QFile(export_path).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
			else
				qfWarning() << "File:" << export_path << "exists already, it will not be overwritten.";
		}
	}
	setCustomReportsDirectory(custom_reports_dir_name);
}

static const auto KEY_customReportsDirectory = QStringLiteral("customReportsDirectory");
//static const auto KEY_exportReportDefinitionsDir = QStringLiteral("exportReportDefinitionsDir");

void ReportsSettingsPage::load()
{
	ui->edCustomReportsDirectory->setText(customReportsDirectory());
}

void ReportsSettingsPage::save()
{
	ReportsSettings settings;
	auto dir = ui->edCustomReportsDirectory->text().trimmed();
	settings.setCustomReportsDirectory(dir);
	qf::qmlwidgets::framework::Plugin::setCustomReportsDir(dir);
}

QString ReportsSettingsPage::customReportsDirectory() const
{
	ReportsSettings settings;
	return settings.customReportsDirectory();
}

}

