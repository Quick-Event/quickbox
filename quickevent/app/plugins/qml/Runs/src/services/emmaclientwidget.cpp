#include "emmaclientwidget.h"
#include "ui_emmaclientwidget.h"
#include "emmaclient.h"

#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/assert.h>

#include <QFileDialog>
#include <QSettings>

namespace services {

ResultsExporterWidget::ResultsExporterWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::EmmaClientWidget)
{
	setPersistentSettingsId("EmmaClientWidget");
	ui->setupUi(this);

	ResultsExporter *svc = service();
	if(svc) {
		ui->edFileName->setText(svc->fileName());
	}

	connect(ui->btChooseFile, &QPushButton::clicked, this, &ResultsExporterWidget::onBtChooseFileClicked);
}

ResultsExporterWidget::~ResultsExporterWidget()
{
	delete ui;
}

void ResultsExporterWidget::onBtChooseFileClicked()
{
	QString fn = QFileDialog::getOpenFileName(this, tr("Open Image"), ui->edFileName->text(), tr("Text Files (*.txt)"));
	if(!fn.isEmpty())
		ui->edFileName->setText(fn);
}

bool ResultsExporterWidget::acceptDialogDone(int result)
{
	if(result == QDialog::Accepted) {
		QString fn = ui->edFileName->text().trimmed();
		if(!fn.isEmpty()) {
			QFile file(fn);
			if(!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
				qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("File '%1' cannot be open for writing.").arg(fn));
				return false;
			}
		}
		Service *svc = service();
		if(svc) {
			QSettings settings;
			settings.beginGroup(svc->settingsGroup());
			settings.setValue(ResultsExporter::SETTING_KEY_FILE_NAME, fn);
			svc->loadSettings();
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
