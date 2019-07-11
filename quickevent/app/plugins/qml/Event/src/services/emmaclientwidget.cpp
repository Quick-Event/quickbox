#include "emmaclientwidget.h"
#include "ui_emmaclientwidget.h"
#include "emmaclient.h"

#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/assert.h>

#include <QFileDialog>

namespace services {

EmmaClientWidget::EmmaClientWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::EmmaClientWidget)
{
	setPersistentSettingsId("EmmaClientWidget");
	ui->setupUi(this);

	EmmaClient *svc = service();
	if(svc) {
		EmmaClientSettings ss = svc->settings();
		ui->edExportDir->setText(ss.exportDir());
		ui->edFileName->setText(ss.fileName());
	}

	connect(ui->btChooseExportDir, &QPushButton::clicked, this, &EmmaClientWidget::onBtChooseExportDirClicked);
	connect(ui->btExportSplits, &QPushButton::clicked, this, &EmmaClientWidget::onBtExportSplitsClicked);
}

EmmaClientWidget::~EmmaClientWidget()
{
	delete ui;
}

void EmmaClientWidget::onBtChooseExportDirClicked()
{
	EmmaClient *svc = service();
	if(svc) {
		EmmaClientSettings ss = svc->settings();
		QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ss.exportDir(), QFileDialog::ShowDirsOnly);
		if(!dir.isEmpty())
			ui->edExportDir->setText(dir);
	}
}

void EmmaClientWidget::onBtExportSplitsClicked()
{
	EmmaClient *svc = service();
	if(svc) {
		saveSettings();
		svc->exportRadioCodes();
		svc->exportResultsIofXml3();
	}
}

bool EmmaClientWidget::acceptDialogDone(int result)
{
	if(result == QDialog::Accepted) {
		/*
		if(!fn.isEmpty()) {
			QFile file(fn);
			if(!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
				qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("File '%1' cannot be open for writing.").arg(fn));
				return false;
			}
		}
		*/
		saveSettings();
	}
	return true;
}

EmmaClient *EmmaClientWidget::service()
{
	EmmaClient *svc = qobject_cast<EmmaClient*>(Service::serviceByName(EmmaClient::serviceName()));
	QF_ASSERT(svc, EmmaClient::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

void EmmaClientWidget::saveSettings()
{
	EmmaClient *svc = service();
	if(svc) {
		EmmaClientSettings ss = svc->settings();
		ss.setExportDir(ui->edExportDir->text());
		ss.setFileName(ui->edFileName->text().trimmed());
		svc->setSettings(ss);
	}
}

}
