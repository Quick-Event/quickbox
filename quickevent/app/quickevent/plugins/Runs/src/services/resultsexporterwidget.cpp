#include "resultsexporterwidget.h"
#include "ui_resultsexporterwidget.h"
#include "resultsexporter.h"

#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/assert.h>

#include <QFileDialog>
#include <QSettings>

namespace Runs {
namespace services {

ResultsExporterWidget::ResultsExporterWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::ResultsExporterWidget)
{
	setPersistentSettingsId("ResultsExporterWidget");
	ui->setupUi(this);

	ui->lstOutputFormat->addItem(tr("HTML multi page"), static_cast<int>(ResultsExporterSettings::OutputFormat::HtmlMulti));
	ui->lstOutputFormat->addItem(tr("CSOS fixed column sizes"), static_cast<int>(ResultsExporterSettings::OutputFormat::CSOS));
	ui->lstOutputFormat->addItem(tr("CSV one file"), static_cast<int>(ResultsExporterSettings::OutputFormat::CSV));
	ui->lstOutputFormat->addItem(tr("CSV multi file (file per class)"), static_cast<int>(ResultsExporterSettings::OutputFormat::CSVMulti));
	ui->lstOutputFormat->addItem(tr("IOF-XML 3.0"), static_cast<int>(ResultsExporterSettings::OutputFormat::IofXml3));
	ui->lstOutputFormat->setCurrentIndex(-1);

	ResultsExporter *svc = service();
	if(svc) {
		ResultsExporterSettings ss = svc->settings();
		ui->edExportDir->setText(ss.exportDir());
		ui->edExportInterval->setValue(ss.exportIntervalSec());
		ui->edWhenFinishedRunCmd->setText(ss.whenFinishedRunCmd());
		int of = ss.outputFormat();
		ui->lstOutputFormat->setCurrentIndex(ui->lstOutputFormat->findData(of));
		if (ss.csvSeparator() == QChar::Tabulation)
			ui->rbCsvSeparatorTab->setChecked(true);
		else
		{
			ui->rbCsvSeparatorChar->setChecked(true);
			if (!ss.csvSeparator().isNull())
				ui->edCsvSeparator->setText(ss.csvSeparator());
		}
	}

	connect(ui->btChooseExportDir, &QPushButton::clicked, this, &ResultsExporterWidget::onBtChooseExportDirClicked);
	connect(ui->btExportResults, &QPushButton::clicked, this, &ResultsExporterWidget::onBtExportResultsClicked);
}

ResultsExporterWidget::~ResultsExporterWidget()
{
	delete ui;
}

void ResultsExporterWidget::onBtChooseExportDirClicked()
{
	ResultsExporter *svc = service();
	if(svc) {
		ResultsExporterSettings ss = svc->settings();
		QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ss.exportDir(), QFileDialog::ShowDirsOnly);
		if(!dir.isEmpty())
			ui->edExportDir->setText(dir);
	}
}

void ResultsExporterWidget::onBtExportResultsClicked()
{
	ResultsExporter *svc = service();
	if(svc) {
		saveSettings();
		svc->exportResults();
	}
}

bool ResultsExporterWidget::saveSettings()
{
	ResultsExporter *svc = service();
	if(svc) {
		ResultsExporterSettings ss = svc->settings();
		QString dir = ui->edExportDir->text().trimmed();
		ss.setExportDir(dir);
		ss.setExportIntervalSec(ui->edExportInterval->value());
		ss.setWhenFinishedRunCmd(ui->edWhenFinishedRunCmd->text());
		if(ui->lstOutputFormat->currentIndex() >= 0)
			ss.setOutputFormat(ui->lstOutputFormat->itemData(ui->lstOutputFormat->currentIndex()).toInt());
		if (ui->rbCsvSeparatorTab->isChecked())
			ss.setCsvSeparator(QChar::Tabulation);
		else if (ui->edCsvSeparator->text().isEmpty())
			ss.setCsvSeparator(QChar::Null);
		else
			ss.setCsvSeparator(ui->edCsvSeparator->text().at(0));
		svc->setSettings(ss);
		if(!dir.isEmpty()) {
			if(!QDir().mkpath(dir))
				return false;
		}
	}
	return true;
}

bool ResultsExporterWidget::acceptDialogDone(int result)
{
	if(result == QDialog::Accepted) {
		if(!saveSettings()) {
			qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("Cannot create directory '%1'.").arg(ui->edExportDir->text().trimmed()));
			return false;
		}
	}
	return true;
}

ResultsExporter *ResultsExporterWidget::service()
{
	ResultsExporter *svc = qobject_cast<ResultsExporter*>(Event::services::Service::serviceByName(ResultsExporter::serviceName()));
	QF_ASSERT(svc, ResultsExporter::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

}}
