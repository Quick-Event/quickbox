#include "emmaclientwidget.h"
#include "ui_emmaclientwidget.h"
#include "emmaclient.h"
#include "../eventplugin.h"
#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/assert.h>

#include <QFileDialog>
using qf::qmlwidgets::framework::getPlugin;

namespace Event {
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
		ui->chExportStartTxt->setCheckState((ss.exportStartTypeTxt()) ? Qt::Checked : Qt::Unchecked);
		ui->chExportFinishTxt->setCheckState((ss.exportFinishTypeTxt()) ? Qt::Checked : Qt::Unchecked);
		ui->chExportStartXML30->setCheckState((ss.exportStartTypeXML3()) ? Qt::Checked : Qt::Unchecked);
		ui->chExportResultsXML30->setCheckState((ss.exportResultTypeXML3()) ? Qt::Checked : Qt::Unchecked);
		if (ui->edFileName->text().isEmpty())
			ui->edFileName->setText(getPlugin<EventPlugin>()->eventName());
	}

	connect(ui->btChooseExportDir, &QPushButton::clicked, this, &EmmaClientWidget::onBtChooseExportDirClicked);
	connect(ui->btExportSplitsTxt, &QPushButton::clicked, this, &EmmaClientWidget::onBtExportSplitsTxtClicked);
	connect(ui->btExportFinishTxt, &QPushButton::clicked, this, &EmmaClientWidget::onBtExportFinishTxtClicked);
	connect(ui->btExportStartTxt, &QPushButton::clicked, this, &EmmaClientWidget::onBtExportStartTxtClicked);
	connect(ui->btExportResultsXML30, &QPushButton::clicked, this, &EmmaClientWidget::onBtExportResultsXML30Clicked);
	connect(ui->btExportStartXML30, &QPushButton::clicked, this, &EmmaClientWidget::onBtExportStartXML30Clicked);
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

void EmmaClientWidget::onBtExportSplitsTxtClicked()
{
	EmmaClient *svc = service();
	if(svc) {
		saveSettings();
		svc->exportRadioCodesRacomTxt();
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
		ss.setExportStartTypeTxt(ui->chExportStartTxt->isChecked());
		ss.setExportFinishTypeTxt(ui->chExportFinishTxt->isChecked());
		ss.setExportStartTypeXML3(ui->chExportStartXML30->isChecked());;
		ss.setExportResultTypeXML3(ui->chExportResultsXML30->isChecked());;
		if (ss.fileName().isEmpty())
			ss.setFileName(getPlugin<EventPlugin>()->eventName());

		svc->setSettings(ss);
		if(!dir.isEmpty()) {
			if(!QDir().mkpath(dir))
				return false;
		}
	}
	return true;
}

void EmmaClientWidget::onBtExportFinishTxtClicked()
{
	EmmaClient *svc = service();
	if(svc) {
		saveSettings();
		svc->exportFinishRacomTxt();
	}
}

void EmmaClientWidget::onBtExportStartTxtClicked()
{
	EmmaClient *svc = service();
	if(svc) {
		saveSettings();
		svc->exportStartListRacomTxt();
	}
}

void EmmaClientWidget::onBtExportResultsXML30Clicked()
{
	EmmaClient *svc = service();
	if(svc) {
		saveSettings();
		svc->exportResultsIofXml3();
	}
}

void EmmaClientWidget::onBtExportStartXML30Clicked()
{
	EmmaClient *svc = service();
	if(svc) {
		saveSettings();
		svc->exportStartListIofXml3();
	}
}
}}

