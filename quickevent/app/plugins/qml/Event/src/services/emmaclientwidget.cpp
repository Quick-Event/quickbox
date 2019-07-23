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
		ui->edExportInterval->setValue(ss.exportIntervalSec());
		ui->chExportStart->setCheckState((ss.exportStart()) ? Qt::Checked : Qt::Unchecked);
		ui->chExportFinish->setCheckState((ss.exportFinish()) ? Qt::Checked : Qt::Unchecked);
		ui->chExportXML30->setCheckState((ss.exportTypeXML3()) ? Qt::Checked : Qt::Unchecked);
	}

	connect(ui->btChooseExportDir, &QPushButton::clicked, this, &EmmaClientWidget::onBtChooseExportDirClicked);
	connect(ui->btExportSplits, &QPushButton::clicked, this, &EmmaClientWidget::onBtExportSplitsClicked);
	connect(ui->btExportFinish, &QPushButton::clicked, this, &EmmaClientWidget::onBtExportFinishClicked);
	connect(ui->btExportStart, &QPushButton::clicked, this, &EmmaClientWidget::onBtExportStartClicked);
	connect(ui->btExportXML30, &QPushButton::clicked, this, &EmmaClientWidget::onBtExportXML30Clicked);
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
	}
}

bool EmmaClientWidget::acceptDialogDone(int result)
{
	if(result == QDialog::Accepted) {
		if(!saveSettings()) {
			qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("Cannot create directory '%1'.").arg(ui->edExportDir->text().trimmed()));
			return false;
		}
	}
	return true;
}

EmmaClient *EmmaClientWidget::service()
{
	EmmaClient *svc = qobject_cast<EmmaClient*>(Service::serviceByName(EmmaClient::serviceName()));
	QF_ASSERT(svc, EmmaClient::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

bool EmmaClientWidget::saveSettings()
{
	EmmaClient *svc = service();
	if(svc) {
		EmmaClientSettings ss = svc->settings();
		QString dir = ui->edExportDir->text().trimmed();
		ss.setExportDir(dir);
		ss.setExportIntervalSec(ui->edExportInterval->value());
		ss.setFileName(ui->edFileName->text().trimmed());
		ss.setExportStart(ui->chExportStart->isChecked());
		ss.setExportFinish(ui->chExportFinish->isChecked());
		ss.setExportTypeXML3(ui->chExportXML30->isChecked());;
		svc->setSettings(ss);
		if(!dir.isEmpty()) {
			if(!QDir().mkpath(dir))
				return false;
		}
	}
	return true;
}

void EmmaClientWidget::onBtExportFinishClicked()
{
	EmmaClient *svc = service();
	if(svc) {
		saveSettings();
		svc->exportFinish();
	}
}

void EmmaClientWidget::onBtExportStartClicked()
{
	EmmaClient *svc = service();
	if(svc) {
		saveSettings();
		svc->exportStartList();
	}
}

void EmmaClientWidget::onBtExportXML30Clicked()
{
	EmmaClient *svc = service();
	if(svc) {
		saveSettings();
		svc->exportResultsIofXml3();
	}
}

}

