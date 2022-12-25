#include "racomclientwidget.h"
#include "ui_racomclientwidget.h"
#include "racomclient.h"

#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/assert.h>

#include <QFileDialog>

namespace CardReader {
namespace services {

RacomClientWidget::RacomClientWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::RacomClientWidget)
{
	setPersistentSettingsId("RacomClientWidget");
	ui->setupUi(this);

	RacomClient *svc = service();
	if(svc) {
		RacomClientSettings ss = svc->settings();
		ui->chkListenRawData->setChecked(ss.isListenRawData());
		ui->edRawDataListenPort->setValue(ss.rawDataListenPort());
		ui->chkListenSirxdData->setChecked(ss.isListenSirxdData());
		ui->edSirxdDataListenPort->setValue(ss.sirxdDataListenPort());
		ui->gbReadSplitsFile->setChecked(ss.isReadSplitFile());
		ui->edReadSplitsFile->setText(ss.splitFileName());
		ui->edReadSplitInterval->setValue(ss.splitFileInterval());
		ui->edReadSplitFinishCode->setValue(ss.splitFileFinishCode());
	}

	connect(ui->btChooseSplitsFile, &QPushButton::clicked, this, &RacomClientWidget::onBtChooseTxtSplitsFileClicked);
}

RacomClientWidget::~RacomClientWidget()
{
	delete ui;
}

bool RacomClientWidget::acceptDialogDone(int result)
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

RacomClient *RacomClientWidget::service()
{
	RacomClient *svc = qobject_cast<RacomClient*>(Event::services::Service::serviceByName(RacomClient::serviceName()));
	QF_ASSERT(svc, RacomClient::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

void RacomClientWidget::saveSettings()
{
	RacomClient *svc = service();
	if(svc) {
		RacomClientSettings ss = svc->settings();
		ss.setListenRawData(ui->chkListenRawData->isChecked());
		ss.setRawDataListenPort(ui->edRawDataListenPort->value());
		ss.setListenSirxdData(ui->chkListenSirxdData->isChecked());
		ss.setSirxdDataListenPort(ui->edSirxdDataListenPort->value());
		ss.setReadSplitFile(ui->gbReadSplitsFile->isChecked());
		ss.setSplitFileName(ui->edReadSplitsFile->text());
		ss.setSplitFileFinishCode(ui->edReadSplitFinishCode->value());
		ss.setSplitFileInterval(ui->edReadSplitInterval->value());
		svc->setSettings(ss);
	}
}

void RacomClientWidget::onBtChooseTxtSplitsFileClicked()
{
	RacomClient *svc = service();
	if(svc) {
		RacomClientSettings ss = svc->settings();
		QString file = QFileDialog::getOpenFileName(this, tr("Open txt splits file"), ss.splitFileName());
		if(!file.isEmpty())
			ui->edReadSplitsFile->setText(file);
	}
}

}}
