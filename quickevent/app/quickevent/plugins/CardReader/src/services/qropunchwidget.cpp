#include "qropunchwidget.h"
#include "ui_qropunchwidget.h"
#include "qropunch.h"

#include <qf/core/assert.h>

#include <QDialog>
#include <QFileDialog>

namespace CardReader {
namespace services {

QrOPunchWidget::QrOPunchWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::QrOPunchWidget)
{
	setPersistentSettingsId("QrOPunchWidget");
	ui->setupUi(this);

	QrOPunch *svc = service();
	if (svc) {
		QrOPunchSettings ss = svc->settings();
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
	connect(ui->btChooseLogFile, &QPushButton::clicked, this, &QrOPunchWidget::onBtChooseLogFileClicked);
}

QrOPunchWidget::~QrOPunchWidget()
{
	delete ui;
}

bool QrOPunchWidget::acceptDialogDone(int result)
{
	if (result == QDialog::Accepted) {
		saveSettings();
	}
	return true;
}

QrOPunch *QrOPunchWidget::service()
{
	QrOPunch *svc = qobject_cast<QrOPunch*>(Event::services::Service::serviceByName(QrOPunch::serviceName()));
	QF_ASSERT(svc, QrOPunch::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

void QrOPunchWidget::saveSettings()
{
	QrOPunch *svc = service();
	if (svc) {
		QrOPunchSettings ss = svc->settings();
		ss.setTcpListenPort(ui->edTcpListenPort->value());
		ss.setWriteLogFile(ui->chkLogRequests->isChecked());
		ss.setLogFileName(ui->edLogFile->text());
		svc->setSettings(ss);
	}
}

void QrOPunchWidget::onBtChooseLogFileClicked()
{
	QrOPunch *svc = service();
	if (svc) {
		QrOPunchSettings ss = svc->settings();
		QString file = QFileDialog::getSaveFileName(this, tr("Choose file to log requests"), ss.logFileName(),
												{}, nullptr, QFileDialog::DontConfirmOverwrite);
		if (!file.isEmpty())
			ui->edLogFile->setText(file);
	}
}

}}
