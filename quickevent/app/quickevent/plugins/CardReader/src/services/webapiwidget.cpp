#include "webapiwidget.h"
#include "ui_webapiwidget.h"
#include "webapi.h"

#include <qf/core/assert.h>

#include <QDialog>
#include <QFileDialog>

namespace CardReader {
namespace services {

WebApiWidget::WebApiWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::WebApiWidget)
{
	setPersistentSettingsId("WebApiWidget");
	ui->setupUi(this);

	WebApi *svc = service();
	if (svc) {
		WebApiSettings ss = svc->settings();
		ui->edTcpListenPort->setValue(ss.tcpListenPort());
		ui->chkLogRequests->setChecked(ss.isWriteLogFile());
		ui->edLogFile->setText(ss.logFileName());
    }
	auto updateLogFileUI = [this] {
		bool isEnabled = ui->chkLogRequests->isChecked();
		ui->btChooseLogFile->setEnabled(isEnabled);
		ui->edLogFile->setEnabled(isEnabled);
	};
	updateLogFileUI();
	connect(ui->chkLogRequests, &QCheckBox::clicked, updateLogFileUI);
	connect(ui->btChooseLogFile, &QPushButton::clicked, this, &WebApiWidget::onBtChooseLogFileClicked);
}

WebApiWidget::~WebApiWidget()
{
	delete ui;
}

bool WebApiWidget::acceptDialogDone(int result)
{
	if (result == QDialog::Accepted) {
		saveSettings();
	}
	return true;
}

WebApi *WebApiWidget::service()
{
	WebApi *svc = qobject_cast<WebApi*>(Event::services::Service::serviceByName(WebApi::serviceName()));
	QF_ASSERT(svc, WebApi::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

void WebApiWidget::saveSettings()
{
	WebApi *svc = service();
	if (svc) {
		WebApiSettings ss = svc->settings();
		ss.setTcpListenPort(ui->edTcpListenPort->value());
		ss.setWriteLogFile(ui->chkLogRequests->isChecked());
		ss.setLogFileName(ui->edLogFile->text());
		svc->setSettings(ss);
	}
}

void WebApiWidget::onBtChooseLogFileClicked()
{
	WebApi *svc = service();
	if (svc) {
		WebApiSettings ss = svc->settings();
		QString file = QFileDialog::getSaveFileName(this, tr("Choose file to log requests"), ss.logFileName(),
		                                            {}, nullptr, QFileDialog::DontConfirmOverwrite);
		if (!file.isEmpty())
			ui->edLogFile->setText(file);
	}
}

}}
