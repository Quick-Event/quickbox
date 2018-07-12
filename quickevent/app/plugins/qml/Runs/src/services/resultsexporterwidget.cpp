#include "resultsexporterwidget.h"
#include "ui_resultsexporterwidget.h"
#include "resultsexporter.h"

#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/assert.h>

#include <QFileDialog>
#include <QSettings>

namespace services {

ResultsExporterWidget::ResultsExporterWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::ResultsExporterWidget)
{
	setPersistentSettingsId("EmmaClientWidget");
	ui->setupUi(this);

	ResultsExporter *svc = service();
	if(svc) {
		ResultsExporterSettings ss = svc->settings();
		ui->edExportDir->setText(ss.exportDir());
		ui->edExportInterval->setValue(ss.exportIntervalSec());
		ui->edWhenFinishedRunCmd->setText(ss.whenFinishedRunCmd());
	}

	connect(ui->btChooseExportDir, &QPushButton::clicked, this, &ResultsExporterWidget::onBtChooseExportDir);
}

ResultsExporterWidget::~ResultsExporterWidget()
{
	delete ui;
}

void ResultsExporterWidget::onBtChooseExportDir()
{
	ResultsExporter *svc = service();
	if(svc) {
		ResultsExporterSettings ss = svc->settings();
		QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ss.exportDir(), QFileDialog::ShowDirsOnly);
		if(!dir.isEmpty())
			ui->edExportDir->setText(dir);
	}
}

bool ResultsExporterWidget::acceptDialogDone(int result)
{
	if(result == QDialog::Accepted) {
		QString dir = ui->edExportDir->text().trimmed();
		if(!dir.isEmpty()) {
			if(!QDir().mkpath(dir)) {
				qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("Cannot create directory '%1'.").arg(dir));
				return false;
			}
		}
		ResultsExporter *svc = service();
		if(svc) {
			ResultsExporterSettings ss = svc->settings();
			ss.setExportDir(dir);
			ss.setExportIntervalSec(ui->edExportInterval->value());
			ss.setWhenFinishedRunCmd(ui->edWhenFinishedRunCmd->text());
			svc->setSettings(ss);
		}
	}
	return true;
}

ResultsExporter *ResultsExporterWidget::service()
{
	ResultsExporter *svc = qobject_cast<ResultsExporter*>(Service::serviceByName(ResultsExporter::serviceName()));
	QF_ASSERT(svc, ResultsExporter::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

}
