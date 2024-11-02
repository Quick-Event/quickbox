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
using qf::qmlwidgets::framework::getPlugin;

namespace Event::services::qx {

QxClientServiceWidget::QxClientServiceWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::QxClientServiceWidget)
{
	setPersistentSettingsId("QxClientServiceWidget");
	ui->setupUi(this);

	auto *svc = service();
	if(svc) {
		auto ss = svc->settings();
		ui->edEventKey->setText(ss.apiKey());
	}
	connect(ui->btGenerateApiKey, &QAbstractButton::clicked, this, [this]() {
		ui->edEventKey->setText(EventPlugin::createApiKey(QxClientService::EVENT_KEY_LEN));
	});
	connect(ui->btRegisterEvent, &QAbstractButton::clicked, this, [this]() {
		QClipboard *clipboard = QGuiApplication::clipboard();
		clipboard->setText(ui->edEventKey->text());
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
		ss.setShvConnectionUrl(ui->edServerUrl->text());
		ss.setApiKey(ui->edEventKey->text().trimmed());
		svc->setSettings(ss);
	}
	return true;
}

}

