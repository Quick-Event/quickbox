#include "qxclientservicewidget.h"
#include "ui_qxclientservicewidget.h"
#include "qxclientservice.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/core/assert.h>

#include <QFileDialog>
#include <QUrlQuery>
#include <QClipboard>

#include <plugins/Event/src/eventplugin.h>

namespace Event::services::qx {

QxClientServiceWidget::QxClientServiceWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::QxClientServiceWidget)
{
	setPersistentSettingsId("QxClientServiceWidget");
	ui->setupUi(this);
	connect(ui->edServerUrl, &QLineEdit::textChanged, this, &QxClientServiceWidget::updateOCheckListPostUrl);

	auto *svc = service();
	if(svc) {
		auto ss = svc->settings();
		ui->edExchangeKey->setText(ss.xchgKey());
		ui->edServerUrl->setText(ss.exchangeServerUrl());
	}
	connect(ui->btRegisterEvent, &QAbstractButton::clicked, this, []() {
	});
}

QxClientServiceWidget::~QxClientServiceWidget()
{
	delete ui;
}

bool QxClientServiceWidget::acceptDialogDone(int result)
{
	if(result == QDialog::Accepted) {
		if(!saveSettings()) {
			return false;
		}
	}
	return true;
}

QxClientService *QxClientServiceWidget::service()
{
	auto *svc = qobject_cast<QxClientService*>(Service::serviceByName(QxClientService::serviceId()));
	QF_ASSERT(svc, QxClientService::serviceId() + " doesn't exist", return nullptr);
	return svc;
}

bool QxClientServiceWidget::saveSettings()
{
	auto *svc = service();
	if(svc) {
		auto ss = svc->settings();
		ss.setExchangeServerUrl(ui->edServerUrl->text());
		svc->setSettings(ss);
	}
	return true;
}

void QxClientServiceWidget::updateOCheckListPostUrl()
{
	auto url = QStringLiteral("%1/api/event/%2/ochecklist")
			.arg(ui->edServerUrl->text())
			.arg(ui->edExchangeKey->text());
	ui->edOChecklistUrl->setText(url);
}

}

